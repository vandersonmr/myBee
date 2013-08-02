/*
 * lib-repa.c
 *
 *  Created on: 20/06/2011
 *      Author: Héberte Fernandes de Moraes
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h> /* for process signals */
#include <stdint.h>
#include <stdbool.h>

#include <sched.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/socket.h>

#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/wireless.h>

#include <errno.h>
#include <time.h>

#define DEBUG 1

#if defined ANDROID && defined DEBUG
#include <android/log.h>
#endif

#include "hdr/repa.h"
#include "hdr/util.h"
#include "hdr/linkedlist.h"

int sock = -1; /* Socket of application */
struct dllist* list_app_interests; /* List app's interest */

pthread_t thread; /* Thread to receive messages and put in buffer list */
bool terminated = false; /* Used in loop of thread */
bool thread_is_running = false; // Indicate if repa_recv_from daemon is running
prefix_addr_t node_add = 0;

u_int16_t node_id = 0;
u_int32_t total_nodes = 0;
struct dllist *node_list = NULL;

#if defined ANDROID && defined DEBUG
const char* TAG = "LIBREPA";
#endif

/* Controls Interests */
#define NUM_CONTROL_INTERESTS 3
#define CONTROL_INTERESTS_TIMESTAMP 0
#define CONTROL_INTERESTS_PING 		1
#define CONTROL_INTERESTS_PONG		2
const char *repa_control_interests[] = {"repad://timestamp", "repad://ping", "repad://pong"};

typedef struct {
	struct dllist *list_msgs; /* List received messages */
	bool wait_mutex;
	pthread_mutex_t mutex; /* Mutex to control access for shared memory */
    sem_t empty;  /* Indicates how many empty positions the buffer has */
    sem_t full;  /* Indicates how many full positions the buffer has */
} message_buffer_t;

message_buffer_t msgs_buff;


#define REPA_MSG_LEN	1500 - sizeof(struct repahdr)

/**
 * TODO List:
 * - Create a receive unblockeable
 * - Test...
 */

/*
 * Thread to receive messages from REPAD daemon
 * and put in a list of messages (list_msgs_received)
 */
void* repa_recv_from_daemon(void* parameter) {
	message_t *msg;
	u_int8_t interest_len = 0;
	u_int16_t data_len = 0;
	prefix_addr_t prefix_addr;

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: Init repa_recv_from_daemon\n");
#endif

	ssize_t read_len = 0;
	char buffer[BUFFER_LEN], interest[BUFFER_LEN], data[BUFFER_LEN];

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: socket is not open\n");
#endif
		return (void*)(-1);
	}

	thread_is_running = true;
	while (!terminated) {
		bzero(buffer, BUFFER_LEN);
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: waiting a message\n");
#endif
		if ((read_len = recvfrom(sock, buffer, BUFFER_LEN, 0, NULL, NULL)) < 0) { // If erros ocurrs
			printf("[LibRepa] ERROR: librepa->recv_from_daemon, exit thread!\n");
#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: ERROR: librepa->recv_from_daemon, exit thread!\n");
#endif
			terminated = true;
			sem_post(&msgs_buff.full); /* Increment the number of messages to release repa_recv */
			pthread_exit(NULL);
			return (void*)(-1);
		}
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: receive a message\n");
#endif

		msg_deserialize(buffer, prefix_addr, interest_len, interest, data_len, data);
		interest[interest_len] = '\0';
		data[data_len] = '\0';

		if (interest_len == 0)
			continue;

		// Initialize the new message received
		msg = (message_t*)malloc(sizeof(message_t));
		bzero(msg, sizeof(message_t));
		msg->data_len = data_len;
		msg->data = (void*)malloc(data_len+1);
		memcpy(msg->data, data, data_len);
		{
			char *aux = (char*)msg->data;
			aux[data_len] = '\0';
		}

		msg->interest = (char*)malloc((interest_len+1)*sizeof(char));
		memcpy(msg->interest, interest, interest_len);
		msg->interest[interest_len] = '\0';

		msg->prefix_addr = prefix_addr;

		/* Producer messages, enter in critical region */
		sem_wait(&msgs_buff.empty); /* Lock empty semaphore if have empty space */
		if (terminated) {
#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: application finished\n");
#endif
			return (void*)(-1);
		}

		msgs_buff.wait_mutex = true;
		pthread_mutex_lock(&msgs_buff.mutex); /* Lock the buffer */
		if (msgs_buff.wait_mutex) {
			msgs_buff.wait_mutex = false;
			dll_push_back(msgs_buff.list_msgs, msg); // Append message in list
		}
		pthread_mutex_unlock(&msgs_buff.mutex); /* Unlock the buffer */
		sem_post(&msgs_buff.full); /* Increment the number of messages */

#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: msg added in buffer\n");
#endif
		/* Exit critical region */
	}

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv_from_daemon: ending\n");
#endif

	thread_is_running = false;
	pthread_exit(NULL);
	return (void*)(0);
}

/*
 * Open a socket to talk with REPAD daemon
 */
int repa_open() {
	char buffer;
	size_t size = 0;
	pthread_attr_t attr;
	struct sched_param sched_param;
	struct sockaddr serv_name;

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: init\n");
#endif

	// repa_open has been called
	if (sock > 0 && thread_is_running) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: has been called\n");
#endif
		return 0;
	}

	// Initialize msgs_buff
	// Create a buffer list of messages received an interests
	dll_create(list_app_interests);
	dll_create(msgs_buff.list_msgs);

	pthread_mutex_init(&msgs_buff.mutex, NULL); // Initialize mutex
	sem_init(&msgs_buff.empty, 0, MAX_BUFFER_MSG); // Initialize empty with MAX_BUFFER_MSG
	sem_init(&msgs_buff.full, 0, 0); // Initialize full is 0 saying have no messages in buffer
	msgs_buff.wait_mutex = false;

	// Open unix raw socket
	if ((sock = socket(AF_LOCAL, SOCK_SEQPACKET, 0)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: error in create socket\n");
#endif
		return -1;
	}

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: socket created\n");
#endif

	// Connect with repad (daemon)
	bzero(&serv_name, sizeof(struct sockaddr));
	serv_name.sa_family = AF_LOCAL;
	strcpy(serv_name.sa_data, REPA_SCK_NAME);
	size = (offsetof(struct sockaddr, sa_data) + strlen(serv_name.sa_data) + 1);

	if (connect(sock, (struct sockaddr *)&serv_name, size) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: error in connect\n");
#endif
		return -1;
	}

	// Request address from daemon
	buffer = TMSG_ASK_ADDR;
	send(sock, &buffer, 1, 0); // Ask node address
	size = recvfrom(sock, &node_add, sizeof(prefix_addr_t), 0, NULL, NULL); // Receive node address

	// Start thread to receive messages from repad
	terminated = false;
	thread_is_running = false;

	/* Create threads */
	sched_param.sched_priority = 10;

	pthread_attr_init(&attr);
	pthread_attr_setschedparam(&attr, &sched_param);
	pthread_create(&thread, &attr, repa_recv_from_daemon, (void*)0); /* Receive message from repad daemon */

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: start thread repa_recv_from_daemon\n");
#endif

	return 0;
}

long app_node_cmp(void *a, void *b, size_t size) {
	app_node_t *app_node_a = (app_node_t*)a;
	app_node_t *app_node_b = (app_node_t*)b;
	int prefix_diff = app_node_a->prefix - app_node_b->prefix;
	int pid_diff = app_node_a->pid - app_node_b->pid;

	return (prefix_diff == 0 ? pid_diff : prefix_diff);
}

void usage(const char* application) {
	printf("Usage: %s [OPTION]\n\n" \
			"\t-n\t\t indicates a total number of nodes running this application in network\n" \
			"\t-h\t\t show this message\n", application);
}

int repa_init(repa_environment_t *env, int argc, char **argv) {
	bool set_total_nodes = false, terminated = false;
	pid_t process_id;
	prefix_addr_t source_address;
	app_node_t *new_app_node, *myself;
	struct dll_node *tmp_node;
	char buffer[BUFFER_LEN], interest[255];
	char *application_name, repa_control_init[255], repa_control_initiated[255];

	struct dll_node *aux = NULL;

	if (env == NULL) {
		printf("[LibRepa] Environment is NULL!\n");
		return -1; // Error, enviroment not created
	}

	if (sock < 0) {
		printf("[LibRepa] repa_open is not called!\n");
		return -1; // Error, repa_open is not callled
	}

	application_name = strrchr(argv[0],'/');
	if (application_name[0] == '/')	application_name++;

	// Manipulate args
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "n:")) != -1) {
		switch (c) {
		case 'n':
			total_nodes = (u_int32_t)atoi(optarg);
			set_total_nodes = true;
			break;
		case 'h':
			usage(application_name);
			exit(0);
			break;
		}
	}

	if (!set_total_nodes) {
		printf("[LibRepa] In this case -n option is mandatory!\n");
		exit(0);
	}


	/* Checks if there are registered interests and temporarily unregister them */
	if (list_app_interests != NULL) {
		if (list_app_interests->num_elements > 0) {
			for (aux = list_app_interests->head; aux != NULL; ) {
				bzero(buffer, BUFFER_LEN);
				buffer[0] = TMSG_INTEREST_UNREG; /* Put a type of message */
				strcpy(buffer+1, (char*)aux->data);
				send(sock, buffer, BUFFER_LEN, 0);
			}
		}
	}

	// Register interest in this application_repa_init
	sprintf(repa_control_init, "%s_%d_repa_init", application_name, total_nodes);
	repa_register_interest(repa_control_init);
	sprintf(repa_control_initiated, "%s_%d_repa_initiated", application_name, total_nodes);
	repa_register_interest(repa_control_initiated);
	process_id = getpid();

	// Create node list
	dll_create(node_list);

	// Add this app_node in list
	myself = (app_node_t*)malloc(sizeof(app_node_t));
	myself->pid = process_id;
	myself->prefix = node_add;
	myself->completed = false;
	dll_sorted_insert(node_list, myself, sizeof(app_node_t), app_node_cmp);

	// Send a ping message
	sprintf(buffer, "ping-%d", process_id);
	repa_send(repa_control_init, buffer, strlen(buffer), 0);

	// Wait for response
	while (!terminated) {
		if (repa_timed_recv(interest, buffer, source_address, (long int)1E9) < 0) { // Wait 1000 milliseconds
			sprintf(buffer, "ping-%d", process_id);
			repa_send(repa_control_init, buffer, strlen(buffer), 0);
		} else {
			new_app_node = (app_node_t*)malloc(sizeof(app_node_t));
			new_app_node->pid = atoi(buffer+5);
			new_app_node->prefix = source_address;
			new_app_node->completed = false;

			tmp_node = dll_has_data(node_list, new_app_node, sizeof(app_node_t), app_node_cmp);
			if (tmp_node == NULL) {
				tmp_node = dll_sorted_insert(node_list, new_app_node, sizeof(app_node_t), app_node_cmp);
			} else {
				free(new_app_node);
			}

			if (strncmp(buffer, "ping", MIN(strlen(buffer),4)) == 0) {
				sprintf(buffer, "pong-%d", process_id);
				repa_send(repa_control_init, buffer, strlen(buffer), 0);
			} else if (strncmp(buffer, "comp", MIN(strlen(buffer),4)) == 0) {
				if (tmp_node != NULL) {
					((app_node_t*)tmp_node->data)->completed = true;
				}
			}

			terminated = true;
			for (aux = node_list->head; aux != NULL; aux = aux->next) {
				new_app_node = (app_node_t*) aux->data;
				terminated &= new_app_node->completed;
			}

			if (node_list->num_elements >= total_nodes) {
				myself->completed = true;
				sprintf(buffer, "comp-%d", process_id);
				repa_send(repa_control_initiated, buffer, strlen(buffer), 0);
			}
		}
	}

	for (aux = node_list->head; aux != NULL; aux = aux->next) {
		new_app_node = (app_node_t*) aux->data;
		if (node_add == new_app_node->prefix && process_id == new_app_node->pid) {
			break;
		} else {
			node_id++;
		}
	}

	if (env != NULL) {
		env->node_id = node_id;
		env->total_nodes = total_nodes;
		sprintf(env->s_node_id, "%d", node_id);
	}

	/*******************************************************************************/
	/* For test */
//	printf("[DEBUG][LibRepa] AppNodeList [%d]:\n", node_id);
//	for (aux = node_list->head; aux != NULL; aux = aux->next) {
//		new_app_node = (app_node_t*) aux->data;
//		printf("\t %d - %d\n", new_app_node->prefix, new_app_node->pid);
//	}
	/*******************************************************************************/

	bzero(interest, 255);
	bzero(buffer, BUFFER_LEN);
	if (node_id == 0) {
		repa_send(repa_control_initiated, "ok", 2, 0);
	} else {
		int result;
		do {
			result = repa_timed_recv(interest, buffer, source_address, (long int)1E9);
		} while ((strcmp(repa_control_initiated, interest) != 0) && (result >=0));
	}

	/* Unregister interest in this application_repa_init */
	repa_unregister_interest(repa_control_init);
	/* Unregister interest in this application_repa_initiated */
	repa_unregister_interest(repa_control_initiated);

	/* Re-registers interests unregister previously */
	if (list_app_interests != NULL) {
		if (list_app_interests->num_elements > 0) {
			for (aux = list_app_interests->head; aux != NULL; ) {
				bzero(buffer, BUFFER_LEN);
				buffer[0] = TMSG_INTEREST_REG; /* Put a type of message */
				strcpy(buffer+1, (char*)aux->data);
				send(sock, buffer, BUFFER_LEN, 0);
			}
		}
	}

	return 0;
}

void repa_clear_buffer() {
	pthread_mutex_lock(&msgs_buff.mutex); /* Lock the buffer */

	dll_destroy_all(msgs_buff.list_msgs);
	msgs_buff.list_msgs = NULL;
	dll_create(msgs_buff.list_msgs);
	sem_init(&msgs_buff.full, 0, 0); // Initialize full with 0 indicating have no messages in buffer

	if (msgs_buff.wait_mutex) {
		sem_init(&msgs_buff.empty, 0, MAX_BUFFER_MSG-1); // Initialize empty with MAX_BUFFER_MSG - 1
	} else {
		sem_init(&msgs_buff.empty, 0, MAX_BUFFER_MSG); // Initialize empty with MAX_BUFFER_MSG
	}
	msgs_buff.wait_mutex = false;
	pthread_mutex_unlock(&msgs_buff.mutex); /* Unlock the buffer */
}

void repa_close() {
	struct dll_node *aux, *tmp;
	message_t *msg;

	terminated = true;
	repa_unregister_all();
	close(sock);
	sock = -1;

	// Clear message buffer
	for (aux = msgs_buff.list_msgs->head; aux != NULL; ) { /* Iterate in Interest list */
		if (aux != NULL) {
			dll_remove_node(msgs_buff.list_msgs, aux);
			tmp = aux;
			aux = aux->next;
			msg = (message_t*)tmp->data;
			if (msg != NULL) {
				if (msg->interest != NULL) free(msg->interest);
				if (msg->data != NULL) free(msg->data);
			}
			if (tmp->data != NULL) free(tmp->data);
			if (tmp != NULL) free(tmp);
		}
	}

	dll_destroy(list_app_interests);
	dll_destroy(msgs_buff.list_msgs);
	list_app_interests = NULL;
	msgs_buff.list_msgs = NULL;

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_close: closing socket...\n");
#endif

	// Release semaphores
	sem_post(&msgs_buff.empty);
	sem_post(&msgs_buff.full);
}

prefix_addr_t repa_node_address() {
	return node_add;
}

/*
 * Register a interest of application
 */
int repa_register_interest(const char* interest) {
	char *interest_reg;
	char buffer[BUFFER_LEN];

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: ERROR: socket is not open\n");
#endif
		return -1;
	}

	if (interest != NULL) {
		if (dll_has_data(list_app_interests, interest, strlen(interest), str_eq_cmp) == NULL) { /* If interest is not found */
			bzero(buffer, BUFFER_LEN);
			buffer[0] = TMSG_INTEREST_REG; /* Put a type of message */
			strcpy(buffer+1, interest);

			if (strlen(interest) > 0) {
				if (send(sock, buffer, BUFFER_LEN, 0) < 0) {
#if defined ANDROID && defined DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: ERROR in communicate with repad\n");
#endif
					return -1;
				}
#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: interest \"%s\" registered\n", interest);
#endif
				interest_reg = strdup(interest);
				interest_reg[strlen(interest)] = '\0';
				dll_append(list_app_interests, interest_reg);
			}
		}
	}

	return 0;
}

/* Unregister a interest of application */
int repa_unregister_interest(const char* interest) {
	char buffer[BUFFER_LEN];
	struct dll_node* aux;

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_interest: ERROR: socket is not open\n");
#endif
		return -1;
	}

	if (interest != NULL) {
		aux = dll_has_data(list_app_interests, interest, strlen(interest), str_eq_cmp); /* Search Interest in list */
		if (aux != NULL) {
			bzero(buffer, BUFFER_LEN);
			buffer[0] = TMSG_INTEREST_UNREG; /* Put a type of message */
			strcpy(buffer+1, interest);
			if (strlen(interest) > 0) {
				if (send(sock, buffer, BUFFER_LEN, 0) < 0) {
#if defined ANDROID && defined DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_interest: ERROR in communicate with daemon\n");
#endif
					return -1;
				}
			}
#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_interest: interest \"%s\" unregistered\n", interest);
#endif
			dll_remove_node(list_app_interests, aux);
			if (aux->data != NULL) free(aux->data);
			if (aux != NULL) free(aux);
		}
	}

	return 0;
}

/* Unregister all interest of application */
int repa_unregister_all() {
	char buffer[BUFFER_LEN];
	struct dll_node *aux, *tmp;

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_all: ERROR: socket is not open\n");
#endif
		return -1;
	}

	for (aux = list_app_interests->head; aux != NULL; ) { /* Iterate in Interest list */
		if (aux != NULL) {
			bzero(buffer, BUFFER_LEN);
			buffer[0] = TMSG_INTEREST_UNREG;
			strcpy(buffer+1, (char*)aux->data);
			if (strlen(aux->data) > 0) {
				if (send(sock, buffer, BUFFER_LEN, 0) < 0) {
#if defined ANDROID && defined DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_all: ERROR in communicate with daemon\n");
#endif
					return -1;
				}
#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_all: interest \"%s\" unregistered\n", (char*)aux->data);
#endif
			}
			dll_remove_node(list_app_interests, aux);
			tmp = aux;
			aux = aux->next;
			if (tmp->data != NULL) free(tmp->data);
			if (tmp != NULL) free(tmp);
		}
	}

	return 0;
}

/*
 * Send a message of interest
 * Parameter are a interest, data, data_len, prefix_address
 *
 * Return:
 * 		N if ok, where N is a number of bytes sent
 * 		-1 if error
 *
 */
ssize_t __repa_send(const char *interest, const void *data, const size_t data_len, const prefix_addr_t dst_addr, bool hide_interest) {
	ssize_t ret = 0;
	u_int8_t interest_len = 0;
	u_int16_t aux_data_len = 0;
	size_t buffer_len;
	char buffer[BUFFER_LEN];

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: init\n");
#endif

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR: socket is not open\n");
#endif
		return -1;
	}

	if (data == NULL) { /* Not a data message */
		errno = EBADMSG;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR: no data to send\n");
#endif
		return -3;
	}

	buffer_len = 0;
	bzero(buffer, BUFFER_LEN);
	interest_len = strlen(interest);
	aux_data_len = (data_len == 0 ? strlen((char*)data) : data_len);

	if (aux_data_len + interest_len > REPA_MSG_LEN) { /* Message too long */
		errno = EMSGSIZE;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR: message too long\n");
#endif
		return -2;
	}

	/* Mount a message to send to REPAD daemon */
	/* Put message type */
	if (hide_interest) buffer[0] = TMSG_SEND_HIDE; /* The interest is hide */
	else buffer[0] = TMSG_SEND; /* The interest is visible by every node */
	msg_serialize(buffer+1, buffer_len, dst_addr, interest_len, interest, aux_data_len, data);

	if ((ret = send(sock, buffer, buffer_len+1, 0)) < 0) { // Send buffer_len + 1 because of message's type
		errno = ESTRPIPE; /* Streams pipe error */
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR in communicate with daemon\n");
#endif
		return -1;
	}
#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: message sent\n");
#endif

	return ret;
}

/*
 * Receive a message
 *
 * Parameters:
 * 		Interest, Data and Source Address of received message
 *
 * Return:
 * 		length of data received
 * 		-1 if error
 *
 */
ssize_t __repa_recv(char *interest, void *data, prefix_addr_t *src_addr) {
	message_t *msg;
	ssize_t length;

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: init\n");
#endif

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: socket is not open\n");
#endif
		return -1;
	}

	if (interest == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: interest is null\n");
#endif
		return -1;
	}

	if (data == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: data is null\n");
#endif
		return -1;
	}

	// Get a message from buffer
	sem_wait(&msgs_buff.full);  /* Lock full semaphore if not zero */
	if (terminated) return -1;

	pthread_mutex_lock(&msgs_buff.mutex); /* Lock the buffer */
	msg = dll_pop_front(msgs_buff.list_msgs); // Get a first message in buffer list
	pthread_mutex_unlock(&msgs_buff.mutex); /* Unlock the buffer */

	sem_post(&msgs_buff.empty); /* Increments semaphore for # of empty */

	if (msg == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: socket breakdown\n");
#endif
		return -1;
	}

	length = msg->data_len;

	// Copy message to user parameter
	strcpy(interest, msg->interest);
	interest[strlen(msg->interest)] = '\0';

	memcpy(data, msg->data, msg->data_len);
	{
		char *aux = (char*)data;
		aux[msg->data_len] = '\0';
	}

	if (src_addr != NULL)
		memcpy(src_addr, &msg->prefix_addr, sizeof(prefix_addr_t));

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: return a message\n");
#endif

	// Free message
	if (msg->interest != NULL) free(msg->interest);
	if (msg->data != NULL) free(msg->data);
	if (msg != NULL) free(msg);

	return length;
}

/*
 * Wait some nanoseconds to receive a message
 *
 * Return:
 * 		length of data received
 * 		-1 if error
 */

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME		0
#endif

#if !defined __timespec_defined && !defined _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC
#define __timespec_defined	1
struct timespec {
	__time_t tv_sec;		/* Seconds.  */
	long int tv_nsec;		/* Nanoseconds.  */
};
#endif

ssize_t __repa_timed_recv(char *interest, void *data, prefix_addr_t *src_addr, const long int nanoseconds) {
	message_t *msg;
	ssize_t length;
	struct timespec abs_timeout;

	if (clock_gettime(CLOCK_REALTIME, &abs_timeout) == -1) {
		printf("[LibRepa] ERROR: Can't clock_gettime (Error No: %d \"%s\")\n", errno, strerror(errno));
	}

	{
		long int nsec = abs_timeout.tv_nsec + (nanoseconds%(long int)1E9);
		long int sec = abs_timeout.tv_sec + (nanoseconds/(long int)1E9);
		sec += nsec/(long int)1E9;
		nsec = nsec%(long int)1E9;
		abs_timeout.tv_nsec = nsec;
		abs_timeout.tv_sec = sec;
	}

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: init\n");
#endif

	if (sock < 0) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: socket is not open\n");
#endif
		return -1;
	}

	if (interest == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: interest is null\n");
#endif
		return -1;
	}

	if (data == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: data is null\n");
#endif
		return -1;
	}

	// Get a message from buffer or wait abstime until return error
	if (sem_timedwait(&msgs_buff.full, &abs_timeout) == -1) {  /* If not a new messages, wait */
//		printf("[LibRepa] ERROR: sem_timedwait Acquiring has_messages (Error No: %d \"%s\")\n", errno, strerror(errno));
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: Acquiring has_messages (Error No: %d \"%s\")\n", errno, strerror(errno));
#endif
		return -2;
	}
	if (terminated) return -1;

	pthread_mutex_lock(&msgs_buff.mutex); /* Lock the buffer */
	msg = dll_pop_front(msgs_buff.list_msgs); // Get a first message in buffer list
	pthread_mutex_unlock(&msgs_buff.mutex); /* Unlock the buffer */

	sem_post(&msgs_buff.empty); /* Increments semaphore for # of empty */

	length = msg->data_len;

	// Copy message to user parameter
	strcpy(interest, msg->interest);

	memcpy(data, msg->data, msg->data_len);
	{
		char *aux = (char*)data;
		aux[msg->data_len] = '\0';
	}

	if (src_addr != NULL)
		memcpy(src_addr, &msg->prefix_addr, sizeof(prefix_addr_t));

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: return  a message\n");
#endif

	// Free message
	if (msg->interest != NULL) free(msg->interest);
	if (msg->data != NULL) free(msg->data);
	if (msg != NULL) free(msg);

	return length;
}

int repa_get_interest_in_network(struct dllist *list) {
	int s = -1;
	ssize_t size = 0;
	u_int8_t num_interests = 0, aux;
	u_int16_t data_pos = 0;
	struct sockaddr saddr;
	char buffer[BUFFER_LEN];
	char *interest = NULL;

	if (list == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: list is null\n");
#endif
		return (-1);
	}

	// Open unix raw socket
	if ((s = socket(AF_LOCAL, SOCK_SEQPACKET, 0)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: error in create socket\n");
#endif
		return -1;
	}

	// Connect with repad (daemon)
	bzero(&saddr, sizeof(struct sockaddr));
	saddr.sa_family = AF_LOCAL;
	strcpy(saddr.sa_data, REPA_SCK_NAME);
	size = (offsetof(struct sockaddr, sa_data) + strlen(saddr.sa_data) + 1);

	if (connect(s, (struct sockaddr*)&saddr, size) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: error in connect\n");
#endif
		return -1;
	}

	// Request address from daemon
	buffer[0] = TMSG_GET_INTERESTS;
	if (send(s, &buffer, 1, 0) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: error in send\n");
#endif
		return -1; // Ask for interests
	}
	if ((size = recvfrom(s, buffer, BUFFER_LEN, 0, NULL, NULL)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: error in recvfrom\n");
#endif
		return -1; // Receive list of interests
	}

	// Mount a list
	num_interests = buffer[0];
	data_pos = 1;
	for (aux = 0; aux < num_interests; aux++) {
		interest = strdup((char*)&buffer[data_pos]);
		data_pos += strlen(interest)+1;
		dll_append(list, interest);
	}

	close(s);
	return 0;
}

int repa_get_nodes_in_network(struct dllist *list) {
	int s = -1;
	ssize_t size = 0;
	u_int8_t num_nodes = 0, aux;
	u_int16_t data_pos = 0;
	struct sockaddr saddr;
	char buffer[BUFFER_LEN];
	prefix_addr_t *prefix;

	if (list == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: list is null\n");
#endif
		return (-1);
	}

	// Open unix raw socket
	if ((s = socket(AF_LOCAL, SOCK_SEQPACKET, 0)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: error in create socket\n");
#endif
		return -1;
	}

	// Connect with repad (daemon)
	bzero(&saddr, sizeof(struct sockaddr));
	saddr.sa_family = AF_LOCAL;
	strcpy(saddr.sa_data, REPA_SCK_NAME);
	size = (offsetof(struct sockaddr, sa_data) + strlen(saddr.sa_data) + 1);

	if (connect(s, (struct sockaddr*)&saddr, size) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: error in connect\n");
#endif
		return -1;
	}

	// Request address from daemon
	buffer[0] = TMSG_GET_NODES;
	if (send(s, &buffer, 1, 0) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: error in send\n");
#endif
		return -1; // Ask for interests
	}
	if ((size = recvfrom(s, buffer, BUFFER_LEN, 0, NULL, NULL)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: error in recvfrom\n");
#endif
		return -1; // Receive list of interests
	}

	// Mount a list
	num_nodes = buffer[0];
	data_pos = 1;
	for (aux = 0; aux < num_nodes; aux++) {
		prefix = (prefix_addr_t*)malloc(sizeof(prefix_addr_t));
		memcpy(prefix, &buffer[data_pos], sizeof(prefix_addr_t));
		data_pos += sizeof(prefix_addr_t);
		dll_append(list, prefix);
	}

	close(s);
	return 0;
}
