/*
 * linkedlist.h
 *
 *  Created on: 15/06/2011
 *      Author: Héberte Fernandes de Moraes
 */

#pragma once

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DLL_THREAD_SAFE

#ifdef DLL_THREAD_SAFE
#include <semaphore.h>
#endif

struct dll_node {
	void* data;
	struct dll_node *next;
	struct dll_node *prev;
};

struct dllist {
	bool sorted;
	uint32_t num_elements;
	struct dll_node *head;
	struct dll_node *tail;
#ifdef DLL_THREAD_SAFE
	sem_t lock;
#endif
};

extern void __dll_create(struct dllist** plist);
#define dll_create(list) __dll_create(&list)

extern struct dll_node* __dll_append(struct dllist* list, void* data);
#define dll_append(list, data) __dll_append(list, (void*) data)

extern void __dll_push_back(struct dllist* list, void* data);
#define dll_push_back(list, data) __dll_push_back(list, (void*) data)

extern void __dll_push_front(struct dllist* list, void* data);
#define dll_push_front(list, data) __dll_push_front(list, (void*) data)

extern void* dll_pop_back(struct dllist* list);
extern void* dll_pop_front(struct dllist* list);

extern struct dll_node* __dll_has_data(struct dllist* list, void* data, size_t data_len, long (*eq_fn)(void*, void*, size_t));
#define dll_has_data(list, data, data_len, eq_fn) __dll_has_data(list, (void*) data, data_len, eq_fn)

extern void __dll_destroy(struct dllist* list);
extern void dll_destroy(void* list);
extern void __dll_destroy_all(struct dllist* list);
extern void dll_destroy_all(void* list);
extern struct dll_node* dll_append_node(struct dllist* list, struct dll_node* lnode);
extern void dll_insert_after_node(struct dllist* list, struct dll_node* lnode, struct dll_node* after);
extern void dll_insert_before_node(struct dllist* list, struct dll_node *lnode, struct dll_node *before);
extern struct dll_node* dll_remove_node(struct dllist* list, struct dll_node* lnode);
extern struct dll_node* dll_pop_back_node(struct dllist* list);
extern struct dll_node* dll_pop_front_node(struct dllist* list);

extern long int_eq_cmp(void *a, void *b, size_t size);
extern long str_eq_cmp(void *a, void *b, size_t size);
extern long mem_eq_cmp(void *a, void *b, size_t size);

// Sorted functions
extern struct dll_node* dll_sorted_insert_node(struct dllist* list, struct dll_node* lnode, size_t data_len, long (*eq_fn)(void*, void*, size_t));
extern struct dll_node* __dll_sorted_insert(struct dllist* list, void* data, size_t data_len, long (*eq_fn)(void*, void*, size_t));
#define dll_sorted_insert(list, data, data_len, eq_fn) __dll_sorted_insert(list, (void*)data, data_len, eq_fn)

// Converter
extern void __dll_to_array(struct dllist* list, void ***array);
#define dll_to_array(list, array) __dll_to_array(list, (void***)array)

#ifdef __cplusplus
}
#endif
#endif /* LINKEDLIST_H_ */
