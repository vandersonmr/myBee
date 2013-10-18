#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "hdr/linkedlist.h"

void dll_insert_before_node(struct dllist* list, struct dll_node *lnode, struct dll_node *before) {
	if (list != NULL) {
		lnode->next = before;
		lnode->prev = before->prev;

		if(before->prev != NULL)
			before->prev->next = lnode;
		else
			list->head = lnode;

		list->num_elements++;
		list->sorted = false;
		before->prev = lnode;
	}
}

struct dll_node* dll_remove_node(struct dllist* list, struct dll_node *lnode) {
	if (lnode == NULL) return NULL;

	if (list != NULL) {
		if(lnode->prev == NULL)
			list->head = lnode->next;
		else
			lnode->prev->next = lnode->next;

		if(lnode->next == NULL)
			list->tail = lnode->prev;
		else
			lnode->next->prev = lnode->prev;

		list->num_elements--;

		return lnode;
	}

	return NULL;
}

struct dll_node* dll_pop_back_node(struct dllist* list) {
	if (list != NULL) {
		struct dll_node *last = list->tail;
		dll_remove_node(list, last);
		return last;
	}
	return NULL;
}

struct dll_node* dll_pop_front_node(struct dllist* list) {
	if (list != NULL) {
		struct dll_node *first = list->head;
		dll_remove_node(list, first);
		return first;
	}
	return NULL;
}

void __dll_create(struct dllist** plist) {
	struct dllist* list = *plist;
	if (list == NULL) {
		list = (struct dllist*)malloc(sizeof(struct dllist));
		list->sorted = false;
		list->head = NULL;
		list->tail = NULL;
		list->num_elements = 0;
		*plist = list;
	}
}

struct dll_node* dll_append_node(struct dllist* list, struct dll_node *lnode) {
	if (list != NULL) {
		if(list->head == NULL) {
			list->head = lnode;
			lnode->prev = NULL;
		} else {
			list->tail->next = lnode;
			lnode->prev = list->tail;
		}

		list->num_elements++;
		list->sorted = false;
		list->tail = lnode;
		lnode->next = NULL;

		return lnode;
	} else {
		return NULL;
	}
}

struct dll_node* __dll_append(struct dllist* list, void* data) {
	if (list != NULL) {
		struct dll_node *lnode;
		lnode = (struct dll_node*)malloc(sizeof(struct dll_node));
		lnode->data = data;
		return dll_append_node(list, lnode);
	} else {
		return NULL;
	}
}

void __dll_push_back(struct dllist* list, void* data) {
	__dll_append(list,data);
}

void __dll_push_front(struct dllist* list, void* data) {
	if (list != NULL) {
		struct dll_node *lnode;
		lnode = (struct dll_node*)malloc(sizeof(struct dll_node));
		lnode->data = data;
		if (list->head == NULL) {
			dll_append_node(list, lnode);
		} else {
			dll_insert_before_node(list,lnode,list->head);
		}
	}
}

void* dll_pop_back(struct dllist* list) {
	if (list != NULL) {
		struct dll_node *last = dll_pop_back_node(list);
		return (last == NULL ? NULL : last->data);
	}
	return NULL;
}

void* dll_pop_front(struct dllist* list) {
	if (list != NULL) {
		struct dll_node *first = dll_pop_front_node(list);
		return (first == NULL ? NULL : first->data);
	}
	return NULL;
}

long int_cmp(void *a, void *b, size_t size) {
	return ((long)a - (long)b);
}

struct dll_node* __dll_has_data(struct dllist* list, void* data, size_t data_len, long (*eq_fn)(void*, void*, size_t)) {
	struct dll_node* aux;

	if (eq_fn == NULL) eq_fn = int_cmp;

	for (aux = list->head; aux != NULL; aux = aux->next) {
		if (eq_fn(aux->data,data,data_len) == 0) {
			return aux;
		}
	}
	return NULL;
}

void dll_insert_after_node(struct dllist* list, struct dll_node *lnode, struct dll_node *after) {
	if (list != NULL) {
		lnode->next = after->next;
		lnode->prev = after;

		if(after->next != NULL)
			after->next->prev = lnode;
		else
			list->tail = lnode;

		list->num_elements++;
		list->sorted = false;
		after->next = lnode;
	}
}

void __dll_destroy(struct dllist* list) {
	if (list != NULL) {
		struct dll_node *lnode;
		/* destroy the dll list */
		while(list->head != NULL) {
			lnode = list->head;
			dll_remove_node(list, list->head);
			free(lnode);
		}
		free(list);
	}
}

void dll_destroy(void* list) {
	__dll_destroy((struct dllist*) list);
}

void __dll_destroy_all(struct dllist* list) {
	if (list != NULL) {
		struct dll_node *lnode;
		/* destroy the dll list */
		while(list->head != NULL) {
			lnode = list->head;
			dll_remove_node(list, list->head);
			free(lnode->data);
			free(lnode);
		}
		free(list);
	}
}

void dll_destroy_all(void* list) {
	__dll_destroy_all((struct dllist*) list);
}

long int_eq_cmp(void *a, void *b, size_t size) { // Compare two integers
	return ((long)a - (long)b);
}

long str_eq_cmp(void *a, void *b, size_t size) { // Compare two char*
	return strcmp((char*)a,(char*)b);
}

long mem_eq_cmp(void *a, void *b, size_t size) { // Compare two space of memory
	return memcmp(a,b,size);
}

struct dll_node* dll_sorted_insert_node(struct dllist* list, struct dll_node* lnode, size_t data_len, long (*eq_fn)(void*, void*, size_t)) {
	struct dll_node* current;

	if (list != NULL) {

		if (list->head == NULL || eq_fn(list->head->data, lnode->data, data_len) >= 0) {
			lnode->next = list->head;
			list->head = lnode;
			list->sorted = true;
		} else {
			current = list->head;
			while (current->next != NULL && eq_fn(current->next->data, lnode->data, data_len) < 0) {
				current = current->next;
			}
			lnode->next = current->next;
			lnode->prev = current;
			current->next = lnode;
		}

		if (lnode->next == NULL) {
			list->tail = lnode;
		}
		list->num_elements++;


		return lnode;
	} else {
		return NULL;
	}
}

struct dll_node* __dll_sorted_insert(struct dllist* list, void* data, size_t data_len, long (*eq_fn)(void*, void*, size_t)) {
	if (list != NULL) {
		struct dll_node *lnode;
		lnode = (struct dll_node*)malloc(sizeof(struct dll_node));
		lnode->prev = NULL;
		lnode->next = NULL;
		lnode->data = data;
		return dll_sorted_insert_node(list, lnode, data_len, eq_fn);
	} else {
		return NULL;
	}
}

void __dll_to_array(struct dllist* list, void ***array) {
	if (array != NULL && list != NULL && list->num_elements > 0) {
		int i;
		struct dll_node *lnode;

		*array = (void**)malloc(list->num_elements*sizeof(list->head->data)); // TODO: fix me, when the pointer is not a valid memory address

		for (lnode = list->head, i = 0; lnode != NULL; lnode = lnode->next, i++) {
			(*array)[i] = lnode->data;
		}
	}
}

//int main(int argc, char *argv[]) {
//	struct dllist *list = NULL;
//	struct dll_node *lnode = NULL;
//	int i = 0;
//
//	dll_create(list);
//	/* add some numbers to the double linked list */
//	for(i = 0; i <= 100; i++) {
//		dll_append(list, (void*)i);
//	}
//
//	/* print the dll list */
//	for(lnode = list->head; lnode != NULL; lnode = lnode->next) {
//		printf("%p\n", lnode->data);
//	}
//
//	int **teste;
//	dll_to_array(list, (void***)&teste);
//	for (i = 0; i < list->num_elements; ++i) {
//		printf("%p\n", teste[i]);
//	}
//
//
//	/* destroy the dll list */
//	dll_destroy(list);
//
//	return 0;
//}
