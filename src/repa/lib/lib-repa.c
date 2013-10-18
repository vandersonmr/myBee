/**
 * Lib-Repa
 *   This is the Repa API implementation, this code is used to communicate
 *   with the Repad Daemon.
 *
 *  Created on: 20/06/2011
 *      Author: Héberte Fernandes de Moraes
 *
 *  Modified on 14/01/2013 by Michael Douglas Barreto e Silva
 *  		To: Include various sockets, comments, consts,
 *  			list nodes in network and local socket not blocking...)
 *  Modified again on 10/04/2013 by Héberte
 *  		To: Change the RepaMessage structure to get both
 *  			source and destination prefix's and added a new
 *  			field to say if the message is S2D or IG.
 *  Modified again on 19/04/2013 by Héberte
 *  		To: Added two socket for connect to daemon
 *  			Change socket type from int to sock_t
 */

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <signal.h> /* for process signals */
#include <stdint.h>
#include <stdbool.h>

#include <sys/socket.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/wireless.h>

#include <errno.h>
#include <time.h>

#include "hdr/repa.h"
#include "hdr/repa-header.h"
#include "hdr/util.h"
#include "hdr/linkedlist.h"
#include "hdr/hashmap.h"

#if defined ANDROID && defined DEBUG
#include <android/log.h>
const char* TAG = "LIBREPA";
#endif

hashmap* hash_app_interests = NULL;

prefix_addr_t repa_node_addr = 0;

uint16_t node_id = 0;
uint32_t total_nodes = 0;
struct dllist *node_list = NULL;

/* Controls Interests */
#define NUM_CONTROL_INTERESTS 3
#define CONTROL_INTERESTS_TIMESTAMP 0
#define CONTROL_INTERESTS_PING 		1
#define CONTROL_INTERESTS_PONG		2
const char *repa_control_interests[] = {"repad://timestamp", "repad://ping", "repad://pong"};

// returns "select" function
#define SELECT_TIMEOUT				0
#define SELECT_ERROR				-1

#if __WORDSIZE == 64
uint64_t sock_hash_fn(key in) {
#else
uint32_t sock_hash_fn(key in) {
#endif
	static repa_sock_t *sock = NULL;
	static uint64_t a = 0;

	sock = (repa_sock_t*)in;
	a = (((uint64_t)sock->sock_recv) << 32) || ((uint64_t)sock->sock_send);

	a = (a+0x7ed55d16) + (a<<12);
	a = (a^0xc761c23c) ^ (a>>19);
	a = (a+0x165667b1) + (a<<5);
	a = (a+0xd3a2646c) ^ (a<<9);
	a = (a+0xfd7046c5) + (a<<3);
	a = (a^0xb55a4f09) ^ (a>>16);

#if __WORDSIZE == 64
	return a;
#else
	return (uint32_t)a;
#endif
}

void list_del_fn(val value) {
	dll_destroy(value);
}

/**
 * Open a socket to talk with REPAD daemon (create socket with REPAD daemon)
 *
 *  .:*@*:. It's very important use this command before use
 *  a repad daemon calls like: repa_send, repa_receive, register_interest...
 *
 * Return:
 * 		A sock_t struct with two sockets if ok
 * 		if a error occur return a this errors:
 * 			ERROR_CREATE_SOCKET,ERROR_CREATE_HASHMAP
 * 			ERROR_CONNECT_SOCKET,ERROR_SEND_SOCKET
 * 			ERROR_RECVFROM_SOCKET
 */
repa_sock_t repa_open() {
	size_t size = 0;
	repa_sock_t *sock;
	struct sockaddr serv_name;
	struct dllist* listAux = NULL;

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: init\n");
#endif

	sock = calloc(1, sizeof(repa_sock_t));

	// Open unix raw socket
	if ((sock->sock_send = socket(AF_LOCAL, SOCK_SEQPACKET, 0)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: error in create socket\n");
#endif
		sock->sock_send = -1;
		sock->sock_recv = -1;
		sock->error = ERROR_CREATE_SOCKET;
		return *sock;
	}

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: socket created\n");
#endif

	// Connect with repad (daemon)
	bzero(&serv_name, sizeof(struct sockaddr));
	serv_name.sa_family = AF_LOCAL;
	strcpy(serv_name.sa_data, REPA_SCK_NAME);
	size = (offsetof(struct sockaddr, sa_data) + strlen(serv_name.sa_data) + 1);

	if (connect(sock->sock_send, (struct sockaddr *)&serv_name, size) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: ERROR in connect function\n");
#endif
		close(sock->sock_send);
		sock->sock_send = -1;
		sock->sock_recv = -1;
		sock->error = ERROR_CONNECT_SOCKET;
		return *sock;
	}

	/* Receive the new socket and prefix address from daemon */
	{
		struct msghdr msg;
		struct iovec iov;

		union {
			struct cmsghdr cmsg;
			char control[CMSG_SPACE(sizeof(int))];
		} control_un;

		msg.msg_control = control_un.control;
		msg.msg_controllen = sizeof(control_un.control);

		msg.msg_name = NULL;
		msg.msg_namelen = 0;

		// Receive node address
		iov.iov_base = &repa_node_addr;
		iov.iov_len = sizeof(prefix_addr_t);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

		if (recvmsg(sock->sock_send, &msg, 0) <= 0) {
#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: ERROR in recvmsg() function on receive new socket\n");
#endif
			close(sock->sock_send);
			sock->sock_send = -1;
			sock->sock_recv = -1;
			sock->error = ERROR_RECV_SOCKET;
			return *sock;
		}

		struct cmsghdr *cmsg_ptr;
		if (((cmsg_ptr = CMSG_FIRSTHDR(&msg)) != NULL) && (cmsg_ptr->cmsg_len == CMSG_LEN(sizeof(int)))) {
			if (cmsg_ptr->cmsg_level != SOL_SOCKET && cmsg_ptr->cmsg_type != SCM_RIGHTS) {
#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: ERROR in recvmsg() function: receive wrong cmsghdr\n");
#endif
				close(sock->sock_send);
				sock->sock_send = -1;
				sock->sock_recv = -1;
				sock->error = ERROR_RECV_SOCKET;
				return *sock;
			}
			memcpy(&sock->sock_recv, CMSG_DATA(cmsg_ptr), sizeof(int));
		} else {
#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: ERROR in recvmsg() function: descriptor was not passed\n");
#endif
			close(sock->sock_send);
			sock->sock_send = -1;
			sock->sock_recv = -1;
			sock->error = ERROR_RECV_SOCKET;
			return *sock;
		}
	}

	// Create hasmap interests
	if(hash_app_interests == NULL) {
		hash_app_interests = mk_hmap(sock_hash_fn,int_eq_fn,list_del_fn);
	}

	dll_create(listAux);

	if(!hmap_add(hash_app_interests, sock,listAux)) {
		close(sock->sock_send);
		close(sock->sock_recv);
		sock->sock_send = -1;
		sock->sock_recv = -1;
		sock->error = ERROR_CREATE_HASHMAP;
		return *sock;
	}

	#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_open: socket open successfully\n");
	#endif

	return *sock;
}

long app_node_cmp(void *a, void *b, size_t size) {
	app_node_t *app_node_a = (app_node_t*)a;
	app_node_t *app_node_b = (app_node_t*)b;
	int prefix_diff = app_node_a->prefix - app_node_b->prefix;
	int pid_diff = app_node_a->pid - app_node_b->pid;

	return (prefix_diff == 0 ? pid_diff : prefix_diff);
}

/**
 * Close communication with REPAD service and clean
 * unregister all interest used by application and
 * close socket
 *
 * Parameter:
 * 		sock_t sock: the opened socket with daemon
 *
 *  .:*@*:. It's very important use this command before
 *  finish application
 */
void repa_close(repa_sock_t sock) {
	repa_unregister_all(sock);

	/* Shutdown all possibilities of communication: read or write.
	 * This will unblock all recvfrom calls. I hope so.
	 */
	shutdown(sock.sock_recv, SHUT_RDWR);
	close(sock.sock_recv);
	shutdown(sock.sock_send, SHUT_RDWR);
	close(sock.sock_send);

	sock.ref_count = 0;
	sock.error = 0;

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_close: closed socket\n");
#endif
}

/**
 * Return prefix address this node
 *
 * Return:
 * 		Node Prefix Address
 */
prefix_addr_t repa_get_node_address() {
	return repa_node_addr;
}

/**
 * Register a interest of application
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 * 		const char* interest : interest to register
 *
 * Return:
 * 		OK if not a error occur
 * 		if a error occur return a this errors:
 * 			ERROR_NOT_OPEN_SOCKET,ERROR_SEND_SOCKET
 */
int repa_register_interest(repa_sock_t sock, const char* interest) {
	uint8_t interest_len = 0;
	char *interest_reg;
	char buffer[BUFFER_LEN];

	struct dllist* listAux = NULL;

#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: register starting...\n");
#endif
	if ((sock.sock_recv < 0) || (sock.sock_send < 0)) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: ERROR: socket is not open\n");
#endif
		sock.error = ERROR_NOT_OPEN_SOCKET;
		return ERROR_NOT_OPEN_SOCKET;
	}

	if (interest != NULL) {
		listAux = hmap_get(hash_app_interests, &sock);
		interest_len = strlen(interest);

		if (interest_len > 0) {
			if (dll_has_data(listAux, interest, interest_len, str_eq_cmp) == NULL) { /* If interest is not found */
				buffer[0] = TMSG_INTEREST_REG; /* Put a type of message */
				strcpy(buffer+1, interest);
				buffer[interest_len+2] = '\0';

				if (send(sock.sock_send, buffer, interest_len+2, 0) < 0) {
#if defined ANDROID && defined DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: ERROR in communicate with repad\n");
#endif
				}
#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_register_interest: interest \"%s\" registered\n", interest);
#endif
				interest_reg = strdup(interest);
				interest_reg[interest_len] = '\0';
				dll_append(listAux, interest_reg);
			}
		}
	}

	return OK;
}

/**
 * Unregister a interest of application
 *
  * Parameters:
 * 		sock_t sock: the opened socket with daemon
 * 		const char* interest : interest to unregister
 *
 * Return:
 * 		OK if not a error occur
 * 		if a error occur return a this errors:
 * 			ERROR_NOT_OPEN_SOCKET,ERROR_SEND_SOCKET
 */
int repa_unregister_interest(repa_sock_t sock, const char* interest) {
	uint8_t interest_len = 0;
	char buffer[BUFFER_LEN];

	struct dll_node* aux;
	struct dllist* listAux;

	if ((sock.sock_recv < 0) || (sock.sock_send < 0)) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_interest: ERROR: socket is not open\n");
#endif
		sock.error = ERROR_NOT_OPEN_SOCKET;
		return ERROR_NOT_OPEN_SOCKET;
	}

	if (interest != NULL) {
		listAux = hmap_get(hash_app_interests, &sock);
		interest_len = strlen(interest);

		if (interest_len > 0) {
			aux = dll_has_data(listAux, interest, interest_len, str_eq_cmp); /* Search Interest in list */
			if (aux != NULL) {
				buffer[0] = TMSG_INTEREST_UNREG; /* Put a type of message */
				strcpy(buffer+1, interest);
				buffer[interest_len+2] = '\0';

				if (send(sock.sock_send, buffer, interest_len+2, 0) < 0) {
#if defined ANDROID && defined DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_interest: ERROR in communicate with daemon\n");
#endif
				}
#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_interest: interest \"%s\" unregistered\n", interest);
#endif
				dll_remove_node(listAux, aux);
				if (aux->data != NULL) free(aux->data);
				if (aux != NULL) free(aux);
			}
		}
	}

	return OK;
}

/**
 * Unregister all interest of application
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 *
 * Return:
 * 		OK if not a error occur
 * 		if a error occur return a this errors:
 * 			ERROR_NOT_OPEN_SOCKET,ERROR_SEND_SOCKET
 */
int repa_unregister_all(repa_sock_t sock) {
	uint8_t interest_len = 0;
	char buffer[BUFFER_LEN];
	struct dll_node *aux, *tmp;
	struct dllist* listAux;

	if ((sock.sock_recv < 0) || (sock.sock_send < 0)) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_all: ERROR: socket is not open\n");
#endif
		sock.error = ERROR_NOT_OPEN_SOCKET;
		return ERROR_NOT_OPEN_SOCKET;
	}

	listAux = hmap_get(hash_app_interests, &sock);
	buffer[0] = TMSG_INTEREST_UNREG;
	for (aux = listAux->head; aux != NULL; ) { /* Iterate in Interest list */
		if (aux != NULL) {
			interest_len = strlen(aux->data);
			strcpy(buffer+1, (char*)aux->data);
			buffer[interest_len+2] = '\0';
			if (interest_len > 0) {
				if (send(sock.sock_send, buffer, interest_len+2, 0) < 0) {
#if defined ANDROID && defined DEBUG
					__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_all: ERROR in communicate with daemon\n");
#endif
				}
#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_unregister_all: interest \"%s\" unregistered\n", (char*)aux->data);
#endif
			}
			dll_remove_node(listAux, aux);
			tmp = aux;
			aux = aux->next;
			if (tmp->data != NULL) free(tmp->data);
			if (tmp != NULL) free(tmp);
		}
	}

	return OK;
}

/**
 * Send a message of interest
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 * 		const char* interest : interest to send message
 * 		const void *data : any data (max: len(data)+len(interest) <= ~1500)
 * 		const size_t data_len : size data
 * 		const prefix_addr_t dst_addr : destination address prefix
 *		bool hide_interest : hide interest (its not appear in list interests network)
 *
 * Return:
 * 		N if ok, where N is a number of bytes sent
 * 		if a error occur return a this errors:
 * 			ERROR_NOT_OPEN_SOCKET,ERROR_NULL_DATA
 * 			ERROR_TOO_LONG_MESSAGE,ERROR_SEND_SOCKET
 * 			ERROR_SEND_SOCKET
 */
ssize_t __repa_send(repa_sock_t sock, const char *interest, const void *data,
		const size_t data_len, const prefix_addr_t dst_address, bool hide_interest) {
	ssize_t result = 0;
	uint8_t interest_len = 0;
	uint16_t aux_data_len = 0;
	size_t buffer_len;
	prefix_addr_t src_address = 0;
	char buffer[BUFFER_LEN];

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: init\n");
#endif

	if ((sock.sock_recv < 0) || (sock.sock_send < 0)) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR: socket is not open\n");
#endif
		sock.error = ERROR_NOT_OPEN_SOCKET;
		return ERROR_NOT_OPEN_SOCKET;
	}

	if (data == NULL) { /* Not a data message */
		errno = EBADMSG;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR: no data to send\n");
#endif
		return ERROR_NULL_DATA;
	}

	buffer_len = 0;
	interest_len = strlen(interest);
	aux_data_len = (data_len == 0 ? strlen((char*)data) : data_len);

	if (aux_data_len + interest_len > REPA_PACKET_LEN) { /* Message too long */
		errno = EMSGSIZE;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR: message too long\n");
#endif
		return ERROR_TOO_LONG_MESSAGE;
	}

	/* Mount a message to send to REPAD daemon */
	/* Put message type */
	buffer[0] = (hide_interest ? TMSG_SEND_HIDE /* The interest is hide */: TMSG_SEND /* The interest is visible by every node */);
	msg_serialize(buffer+1, buffer_len, dst_address, src_address, interest_len, interest, aux_data_len, data);

	if ((result = send(sock.sock_send, buffer, buffer_len+1, 0)) < 0) { // Send buffer_len + 1 because of message's type
		errno = ESTRPIPE; /* Streams pipe error */
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: ERROR in communicate with daemon\n");
#endif
		return ERROR_SEND_SOCKET;
	}
#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_send: message sent\n");
#endif

	return result;
}

/**
 * Receive a message of interest
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 *		char *interest : return interest from message
 *		void *data : return data from message
 *		prefix_addr_t *src_addr : source address prefix
 *
 * Return:
 * 		length of data received
 * 		if a error occur return a this errors:
 * 			ERROR_NOT_OPEN_SOCKET,ERROR_NULL_INTEREST
 * 			ERROR_NULL_DATA,ERROR_RECVFROM_SOCKET
 * 			ERROR_NULL_RECEIVED_INTEREST
 */
ssize_t __repa_recv(repa_sock_t sock, char *interest, void *data, prefix_addr_t *src_address, prefix_addr_t *dst_address) {
	uint8_t interest_len = 0;
	uint16_t data_len = 0;
	prefix_addr_t dst_prefix;
	ssize_t read_len = 0;
	char buffer[BUFFER_LEN];

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: init\n");
#endif

	if ((sock.sock_recv < 0) || (sock.sock_send < 0)) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: socket is not open\n");
#endif
		sock.error = ERROR_NOT_OPEN_SOCKET;
		return ERROR_NOT_OPEN_SOCKET;
	}

	if (interest == NULL) {
		#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: interest is null\n");
		#endif
		return ERROR_NULL_INTEREST;
	}

	if (data == NULL) {
		#if defined ANDROID && defined DEBUG
				__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: data is null\n");
		#endif
		return ERROR_NULL_DATA;
	}

	#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: waiting a message\n");
	#endif


	if ((read_len = recv(sock.sock_recv, buffer, BUFFER_LEN, 0)) <= 0) { // If error ocurrs
		#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: ERROR: recv call!\n");
		#endif
		return ERROR_RECVFROM_SOCKET;
	}
	#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: receive a message\n");
	#endif

	msg_deserialize(buffer, dst_prefix, *src_address, interest_len, interest, data_len, data);
	interest[interest_len] = '\0';
	((char*)data)[data_len] = '\0';

	if (interest_len == 0) {
		return ERROR_NULL_RECEIVED_INTEREST; // INTEREST NULL
	}

	if (dst_address != NULL) {
		memcpy(dst_address, &dst_prefix, sizeof(prefix_addr_t));
	}

	#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_recv: return a new message\n");
	#endif

	return data_len;
}

/**
 * Wait abstime microseconds to receive a message of interest
 * If timeout return ERROR_TIMEOUT_RECEIVE!
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 *		char *interest : return interest from message
 *		void *data : return data from message
 *		prefix_addr_t *src_addr : source address prefix
 *		const long int microseconds : waiting for X microseconds a message
 *
 * Return:
 * 		length of data received if a message is received
 * 		if a error occur return a this errors:
 * 			ERROR_NOT_OPEN_SOCKET,ERROR_NULL_INTEREST
 * 			ERROR_NULL_DATA,ERROR_RECVFROM_SOCKET
 * 			ERROR_NULL_RECEIVED_INTEREST,ERROR_TIMEOUT_RECEIVE // time out
 * 			ERROR_SELECT_DESCRIPTOR
 */

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME		0
#endif

ssize_t __repa_timed_recv(repa_sock_t sock, char *interest, void *data, prefix_addr_t *src_address,
		prefix_addr_t *dst_address, long int microseconds) {
	uint8_t interest_len = 0;
	uint16_t data_len = 0;
	prefix_addr_t dst_prefix;
	ssize_t read_len = 0;
	char buffer[BUFFER_LEN];

	fd_set read_fds;
	int result_select;

	struct timeval timev = {
		.tv_sec = (microseconds/(long int)1E6),
		.tv_usec = (microseconds%(long int)1E6)
	};

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: init\n");
#endif

	if ((sock.sock_recv < 0) || (sock.sock_send < 0)) {
		errno = ENOTSOCK;
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: socket is not open\n");
#endif
		sock.error = ERROR_NOT_OPEN_SOCKET;
		return ERROR_NOT_OPEN_SOCKET;
	}

	if (interest == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: interest is null\n");
#endif
		return ERROR_NULL_INTEREST;
	}

	if (data == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: data is null\n");
#endif
		return ERROR_NULL_DATA;
	}

#if defined ANDROID && defined DEBUG
	__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: waiting a message\n");
#endif

	// Set up the file descriptor set.
	FD_ZERO(&read_fds); // Clear all bits in the "read_fds" bitmap
	FD_SET(sock.sock_recv, &read_fds); // Mark the sock(th) position in "read_fds" bitmap

	// Wait until timeout or data received.
	// select (number of file descriptor, read_bitmap, write_bitmap, error_bitmap, time)
	result_select = select(sock.sock_recv+1, &read_fds, NULL, NULL, &timev);
	if (result_select == SELECT_TIMEOUT) { // Select Timeout
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: WARNING: timeout\n");
#endif
		return ERROR_TIMEOUT_RECEIVE;
	} else if (result_select == SELECT_ERROR) { // Select Error
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: select error (Error No: %d \"%s\")\n", errno, strerror(errno));
#endif
		return ERROR_SELECT_DESCRIPTOR;
	} else if (FD_ISSET(sock.sock_recv, &read_fds)) { // Socket is ready to read
		if ((read_len = recv(sock.sock_recv, buffer, BUFFER_LEN, 0)) <= 0) { // Test if error occurs on recvfrom
#if defined ANDROID && defined DEBUG
			__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: ERROR: recvfrom!\n");
#endif
			return ERROR_RECVFROM_SOCKET;
		}
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: receive a message\n");
#endif

		msg_deserialize(buffer, dst_prefix, *src_address, interest_len, interest, data_len, data);
		interest[interest_len] = '\0';
		((char*)data)[data_len] = '\0';

		if (interest_len == 0) {
			return ERROR_NULL_RECEIVED_INTEREST; // INTEREST NULL
		}

		if (dst_address != NULL) {
			memcpy(dst_address, &dst_prefix, sizeof(prefix_addr_t));
		}

#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_timed_recv: return  a message\n");
#endif

		return data_len;
	}

	return ERROR_UNKNOWN_SOCKET; // unknown socket
}

/**
 * Get a list of interest registered in daemon
 * return the interests in list passed by parameter
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 * 		struct dllist *list : return list of interest
 *
 * Return:
 * 		OK if not a error occur
 * 		if a error occur return a this errors:
 * 			ERROR_NULL_LIST
 */
int repa_get_interests_registered(repa_sock_t sock, struct dllist *list) {
	uint32_t i;

	struct dll_node *itemList;
	struct dllist* listAux;

	if (list == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interests_registered: list is null\n");
#endif
		return ERROR_NULL_LIST;
	}

	listAux = hmap_get(hash_app_interests, &sock);
	if(listAux != NULL) {
		itemList = listAux->head;
		for(i = 0; i < listAux->num_elements; i++) {
			dll_append(list, itemList->data);
			itemList = itemList->next;
		}
	}

	return OK;
}

/**
 * Get a list of interest collected by daemon in NETWORK
 * return the interests in list passed by parameter
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 * 		struct dllist *list : return list of interest
 *
 * Return:
 * 		OK if not a error occur
 * 		if a error occur return a this errors:
 * 			ERROR_NULL_LIST,
 * 			ERROR_SEND_SOCKET,
 *  		ERROR_RECVFROM_SOCKET
 */
int repa_get_interest_in_network(const repa_sock_t sock, struct dllist *list) {
	ssize_t size = 0;
	uint8_t num_interests = 0, aux;
	uint16_t data_pos = 0;

	char buffer[BUFFER_LEN];
	char *interest = NULL;

	if (list == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: list is null\n");
#endif
		return ERROR_NULL_LIST;
	}

	// Request the list of interest from daemon
	buffer[0] = TMSG_GET_INTERESTS;
	if (send(sock.sock_send, &buffer, 1, 0) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: error in send\n");
#endif
		return ERROR_SEND_SOCKET; // Ask for interests
	}
	/* Wait for response by the same socket asked, because the application
	 * can be using the repa_recv function and the answer can goes to there */
	if ((size = recv(sock.sock_send, buffer, BUFFER_LEN, 0)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_interest_in_network: error in recvfrom\n");
#endif
		return ERROR_RECVFROM_SOCKET; // Receive list of interests
	}

	// Mount a list
	num_interests = buffer[0];
	data_pos = 1;
	for (aux = 0; aux < num_interests; aux++) {
		interest = strdup((char*)&buffer[data_pos]);
		data_pos += strlen(interest)+1;
		dll_append(list, interest);
	}

	return OK;
}

/**
 * Get a list of nodes collected by daemon
 * return the nodes' prefix in list passed by parameter
 *
 * Parameters:
 * 		sock_t sock: the opened socket with daemon
 * 		struct dllist *list : return list of nodes
 *
 * Return:
 * 		OK if not a error occur
 * 		if a error occur return a this errors:
 * 			ERROR_NULL_LIST,
 * 			ERROR_SEND_SOCKET,
 * 			ERROR_RECVFROM_SOCKET
 */
int repa_get_nodes_in_network(const repa_sock_t sock, struct dllist *list) {
	ssize_t size = 0;
	uint8_t num_nodes = 0, aux;
	uint16_t data_pos = 0;

	char buffer[BUFFER_LEN];
	prefix_addr_t *prefix;

	if (list == NULL) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: list is null\n");
#endif
		return ERROR_NULL_LIST;
	}

	// Request address from daemon
	buffer[0] = TMSG_GET_NODES;
	if (send(sock.sock_send, &buffer, 1, 0) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: error in send\n");
#endif
		return ERROR_SEND_SOCKET; // Ask for interests
	}
	/* Wait for response by the same socket asked, because the application
	 * can be using the repa_recv function and the answer can goes to there */
	if ((size = recv(sock.sock_send, buffer, BUFFER_LEN, 0)) < 0) {
#if defined ANDROID && defined DEBUG
		__android_log_print(ANDROID_LOG_DEBUG, TAG, "repa_get_nodes_in_network: error in recvfrom\n");
#endif
		return ERROR_RECVFROM_SOCKET; // Receive list of interests
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

	return OK;
}
