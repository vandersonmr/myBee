/*
 * repa.h
 *
 *  Created on: 20/06/2011
 *      Author: Héberte Fernandes de Moraes
 */

#include <stddef.h>
#include <sys/types.h>

#ifndef __bool_true_false_are_defined
#include <stdbool.h>
#endif

#ifndef __REPA_H__
#define __REPA_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifndef PREFIX_ADDRESS
#define PREFIX_ADDRESS
typedef u_int32_t prefix_addr_t;
#endif

/***************** For REP HPC ***********************/
typedef struct {
	u_int32_t total_nodes;
	u_int16_t node_id;
	char s_node_id[10];
} repa_environment_t;

typedef struct {
	prefix_addr_t prefix;
	pid_t pid;
	bool completed;
} app_node_t;

extern 	u_int16_t node_id;
extern u_int32_t total_nodes;
extern struct dllist *node_list;
/*****************************************************/

extern prefix_addr_t node_add;

extern void repa_print_prefix(u_int32_t address, char *prefix_result);

extern prefix_addr_t repa_node_address(void);

/*
 * Register a interest of application
 */
extern int repa_register_interest(const char* interest);

/*
 * Unregister a interest of application
 */
extern int repa_unregister_interest(const char* interest);

/*
 * Unregister all interest of application
 */
extern int repa_unregister_all(void);

/*
 * Send a message of interest
 *
 * Return:
 * 		0 if ok
 * 		-1 if error
 *
 */
extern ssize_t __repa_send(const char *interest, const void *data, const size_t data_len, const prefix_addr_t dst_addr, bool hide_interest);
#define repa_send(interest, data, data_len, dst_addr) __repa_send((const char*)interest, (const void*)data, (const size_t)data_len, (const prefix_addr_t)dst_addr, false)
#define repa_send_hidden(interest, data, data_len, dst_addr) __repa_send((const char*)interest, (const void*)data, (const size_t)data_len, (const prefix_addr_t)dst_addr, true)


/*
 * Receive a message of interest
 *
 * Return:
 * 		length of data received
 * 		-1 if error
 */
extern ssize_t __repa_recv(char *interest, void *data, prefix_addr_t *src_addr);
#define repa_recv(interest, data, src_addr) __repa_recv((char*)interest, (void*)data, &src_addr)

/*
 * Wait abstime nanoseconds to receive a message of interest
 *
 * Return:
 * 		length of data received
 * 		-1 if error
 */
extern ssize_t __repa_timed_recv(char *interest, void *data, prefix_addr_t *src_addr, const long int nanoseconds);
#define repa_timed_recv(interest, data, src_addr, nanoseconds) __repa_timed_recv((char*)interest, (char*)data, &src_addr, nanoseconds)

/*
 * Close communication with REPAD service and clean
 * unregister all interest used by application and
 * clean buffer list of messages and kill a thread
 * receiving messages
 *
 *  .:*@*:. It's very important use this command before
 *  finish application
 */
extern void repa_close(void);

/*
 * Open communication with REPAD service and create
 * a buffer list of messages and start a thread
 * receiving messages
 *
 *  .:*@*:. It's very important use this command before use
 *  a repad daemon
 */
extern int repa_open(void);

/*
 * Discovers all nodes in network and enumerates them
 *
 */
extern int repa_init(repa_environment_t *env, int argc, char **argv);

/*
 * Clears the receive buffer of messages
 */

extern void repa_clear_buffer(void);

/*
 * Get a list of interest collected by daemon
 * return the interests in list passed by parameter
 *
 * Return:
 * 		0 if ok
 * 		-1 if a error occur
 */
struct dllist;
extern int repa_get_interest_in_network(struct dllist *list);

/*
 * Get a list of nodes collected by daemon
 * return the nodes' prefix in list passed by parameter
 *
 * Return:
 * 		0 if ok
 * 		-1 if a error occur
 */
extern int repa_get_nodes_in_network(struct dllist *list);

#ifdef __cplusplus
}
#endif
#endif /* __REPA_H__ */
