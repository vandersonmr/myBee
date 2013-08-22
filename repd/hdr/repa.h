/*
 * repa.h
 *
 *  Created on: 20/06/2011
 *      Author: Héberte Fernandes de Moraes
 */

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#ifndef __bool_true_false_are_defined
#include <stdbool.h>
#endif

#ifndef __REPA_H__
#define __REPA_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Max size of a Repa packet, including both interest + data */
#ifndef REPA_PACKET_LEN
#define REPA_PACKET_LEN 	1476
#endif

/* Max size of interest in bytes/character */
#define MAX_INTEREST_LEN	255

/* Type for a prefix address */
#ifndef PREFIX_ADDRESS
#define PREFIX_ADDRESS
typedef uint32_t prefix_addr_t;
#endif

#if defined ANDROID && defined DEBUG
const char* TAG;
#endif


/***************** For REP HPC ***********************/
typedef struct repa_environment {
	uint32_t total_nodes;
	uint16_t node_id;
	char s_node_id[10];
} repa_environment_t;

typedef struct app_node {
	prefix_addr_t prefix;
	pid_t pid;
	bool completed;
} app_node_t;

extern uint16_t node_id;
extern uint32_t total_nodes;
extern struct dllist *node_list;

/*****************************************************/


/* ****************************** Errors ***************************** */
/* ******************************************************************* */
/* ! This errors is equal RepaSocketException.java , so modify BOTH  ! */
/*                 !!!!!!! Comments, too !!!!!!!                       */
/* !   And describe in the RepaSocketException.getErrorMessage()     ! */
/* ******************************************************************* */
/* ******************************************************************* */
//model "#define ERROR_[action/information invalid]_[where?]		[value]"
#define OK							 0
#define ERROR_CREATE_SOCKET			-1
#define ERROR_CONNECT_SOCKET		-2
#define ERROR_NOT_OPEN_SOCKET		-3
#define ERROR_SEND_SOCKET			-4
#define ERROR_RECVFROM_SOCKET		-5
#define ERROR_NULL_DATA				-6 // Param null
#define ERROR_NULL_LIST				-7 // Param null
#define ERROR_NULL_INTEREST			-8 // Param null
#define ERROR_NULL_RECEIVED_INTEREST -9 // Received message with null interest
#define ERROR_CREATE_HASHMAP		-10
#define ERROR_TOO_LONG_MESSAGE		-11
#define ERROR_TIMEOUT_RECEIVE		-12
#define ERROR_SELECT_DESCRIPTOR		-13
#define ERROR_UNKNOWN_SOCKET		-14
// Added in 19/04/2013, need to put in RepaSocketException.java
#define ERROR_RECV_SOCKET			-15
#define ERROR_JNI_FAILURE			-16
/* ******************************************************************* */

extern prefix_addr_t repa_node_addr;

/* Type definition of socket, used to send/receive message from applications */
typedef struct repa_sock {
	int ref_count, error;
	int sock_recv, sock_send;
	socklen_t address_len;
	struct sockaddr address;
} repa_sock_t;

/*
 * Convert prefix adress in prefix address string format
 *
 * Parameters:
 * 		uint32_t address : prefix address
 * 		char *prefix_result : return prefix address string format
 *
 */
extern void repa_print_prefix(uint32_t address, char *prefix_result);

/*
 * Return prefix address this node
 *
 * Return:
 * 		Node Prefix Address
 */
extern prefix_addr_t repa_get_node_address(void);

/*
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
extern int repa_register_interest(const repa_sock_t sock, const char* interest);


/*
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
extern int repa_unregister_interest(const repa_sock_t sock, const char* interest);


/*
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
extern int repa_unregister_all(const repa_sock_t sock);


/*
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
extern ssize_t __repa_send(const repa_sock_t sock, const char *interest, const void *data, const size_t data_len, const prefix_addr_t dst_addr, bool hide_interest);
#define repa_send(sock, interest, data, data_len, dst_addr) \
	__repa_send(sock, (const char*)interest, (const void*)data, \
			(const size_t)data_len, (const prefix_addr_t)dst_addr, false)
#define repa_send_hidden(sock, interest, data, data_len, dst_addr) \
	__repa_send(sock, (const char*)interest, (const void*)data, \
			(const size_t)data_len, (const prefix_addr_t)dst_addr, true)


/*
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
extern ssize_t __repa_recv(const repa_sock_t sock, char *interest, void *data, prefix_addr_t *src_address, prefix_addr_t *dst_address);
#define repa_recv(sock, interest, data, src_address) __repa_recv(sock,(char*)interest, (void*)data, &src_address, NULL)

/*
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
extern ssize_t __repa_timed_recv(const repa_sock_t sock, char *interest, void *data,
		prefix_addr_t *src_addr, prefix_addr_t *dst_address, long int microseconds);
#define repa_timed_recv(sock, interest, data, src_address, microseconds) \
	__repa_timed_recv(sock,(char*)interest, (char*)data, &src_address, NULL, microseconds)


/*
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
extern void repa_close(const repa_sock_t sock);


/*
 * Open a socket to talk with REPAD daemon (create socket with REPAD daemon)
 *
 *  .:*@*:. It's very important use this command before use
 *  a repad daemon
 *
 * Return:
 * 		A sock_t struct with two sockets if ok
 * 		if a error occur return a this errors:
 * 			ERROR_CREATE_SOCKET,ERROR_CREATE_HASHMAP
 * 			ERROR_CONNECT_SOCKET,ERROR_SEND_SOCKET
 * 			ERROR_RECVFROM_SOCKET
 */
extern repa_sock_t repa_open(void);


/*
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
 * 			ERROR_NULL_LIST,ERROR_CREATE_SOCKET
 * 			ERROR_CONNECT_SOCKET,ERROR_SEND_SOCKET
 *  		ERROR_RECVFROM_SOCKET
 */
struct dllist;
extern int repa_get_interest_in_network(const repa_sock_t sock, struct dllist *list);

/*
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
extern int repa_get_interests_registered(const repa_sock_t sock, struct dllist *list);

/*
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
 * 			ERROR_NULL_LIST,ERROR_CREATE_SOCKET
 * 			ERROR_CONNECT_SOCKET,ERROR_SEND_SOCKET
 * 			ERROR_RECVFROM_SOCKET
 */
extern int repa_get_nodes_in_network(const repa_sock_t sock, struct dllist *list);


#ifdef __cplusplus
}
#endif
#endif /* __REPA_H__ */
