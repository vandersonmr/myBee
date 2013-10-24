/*
 * hashmap.c
 * Copyright (c) 2009
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "hdr/hashmap.h"
#include <syslog.h>




// hashmaps need a hash function, an equality function, and a destructor
hashmap* mk_hmap(
#if __WORDSIZE == 64
				uint64_t (*hash_fn)(key),
#else
				uint32_t (*hash_fn)(key),
#endif
				bool (*eq_fn)(key, key)
#ifdef HMAP_DESTRUCTORS
				, void (*del_fn)(val)
#endif
	) {
					
	hashmap* hmap = (hashmap*) malloc(sizeof(hashmap));
	hmap->map = (key_val_map*) malloc(sizeof(key_val_map) * HMAP_PRESET_SIZE);
	bzero(hmap->map, sizeof(key_val_map)*HMAP_PRESET_SIZE);
	hmap->size = 0;
	hmap->capacity = HMAP_PRESET_SIZE;
	hmap->hash_fn = hash_fn;
	hmap->eq_fn = eq_fn;
#ifdef HMAP_DESTRUCTORS
	hmap->del_fn = del_fn;
#endif
#ifdef HMAP_THREAD_SAFE
	sem_init(&hmap->lock, 0, 1);
#endif
	return hmap;
}

void free_hmap(hashmap* hmap) {
#ifdef HMAP_THREAD_SAFE
	sem_wait(&hmap->lock);
#endif

#ifdef HMAP_DESTRUCTORS
	static uint32_t it;
	if (hmap->del_fn != NULL) {
		for (it=0; it < hmap->size; ++it) {
			if (hmap->map[it].v != NULL) {
				hmap->del_fn(hmap->map[it].v);
			}
		}
	}
#endif

	free(hmap->map);
	
#ifdef HMAP_THREAD_SAFE
	sem_post(&hmap->lock);
#endif

	free(hmap);
}

static void __oa_hmap_add(key_val_map* map, uint32_t capacity,
#if __WORDSIZE == 64
                          uint64_t (*hash_fn)(key),
#else
                          uint32_t (*hash_fn)(key),
#endif
                          key in, val out) {
#if __WORDSIZE == 64
	uint64_t hash = 0, in_hash = 0;
#else
	uint32_t hash = 0, in_hash = 0;
#endif
	in_hash = hash_fn(in);
	hash = in_hash % capacity;
	
	while (map[hash].v != NULL) {
		hash = (hash + 1) % capacity;
	}

	map[hash].k = (key)in_hash;
	map[hash].v = out;
}

bool __hmap_add(hashmap* hmap, const key in, val out) {
#ifdef HMAP_THREAD_SAFE
	sem_wait(&hmap->lock);
#endif

	// Performance degrades after a certain load
	if (((float) hmap->size) / hmap->capacity > 0.70) {
		key_val_map* temp = (key_val_map*) malloc(hmap->capacity * HMAP_GROWTH_RATE);
		uint32_t old_capacity = hmap->capacity;
		if (temp != NULL) {
			bzero(temp, hmap->capacity * HMAP_GROWTH_RATE);
			hmap->capacity *= HMAP_GROWTH_RATE;
		} else {
		#ifdef HMAP_THREAD_SAFE
			sem_post(&hmap->lock);
		#endif
			// we're out of memory
			return false;
		}
		
		// re-posn all elements
		static uint32_t it;
		for (it=0; it < old_capacity; ++it) {
			if (hmap->map[it].v != NULL) {
				__oa_hmap_add(temp, hmap->capacity, node_hash_fn, hmap->map[it].k, hmap->map[it].v);
			}
		}
		
		// swap out the old map with the new one
		free(hmap->map);
		hmap->map = temp;
	}
	
	__oa_hmap_add(hmap->map, hmap->capacity, hmap->hash_fn, in, out);
	hmap->size += 1;

#ifdef HMAP_THREAD_SAFE
	sem_post(&hmap->lock);
#endif

	return true;
}

val __hmap_get(hashmap* hmap, const key in) {
#ifdef HMAP_THREAD_SAFE
	sem_wait(&hmap->lock);
#endif
#if __WORDSIZE == 64
	static uint64_t hash = 0, in_hash;
#else
	static uint32_t hash = 0, in_hash;
#endif
	in_hash = hmap->hash_fn(in);
	hash = in_hash % hmap->capacity;
	
	while (hmap->map[hash].v != NULL) {
		if (hmap->eq_fn((key)in_hash, hmap->map[hash].k)) {
		#ifdef HMAP_THREAD_SAFE
			sem_post(&hmap->lock);
		#endif			
			return hmap->map[hash].v;
		}
		
		hash = (hash + 1) % hmap->capacity;
	}
	
#ifdef HMAP_THREAD_SAFE
	sem_post(&hmap->lock);
#endif
	return NULL;
}

bool __hmap_set(hashmap* hmap, const key in, val out) {
#ifdef HMAP_THREAD_SAFE
	sem_wait(&hmap->lock);
#endif
#if __WORDSIZE == 64
	static uint64_t hash = 0, in_hash;
#else
	static uint32_t hash = 0, in_hash;
#endif
	in_hash = hmap->hash_fn(in);
	hash = in_hash % hmap->capacity;

	while (hmap->map[hash].v != NULL) {
		if (hmap->eq_fn((key)in_hash, hmap->map[hash].k)) {
			if (hmap->del_fn != NULL)
				hmap->del_fn(hmap->map[hash].v);
			hmap->map[hash].v = out;
#ifdef HMAP_THREAD_SAFE
			sem_post(&hmap->lock);
#endif
			return true;
		}

		hash = (hash + 1) % hmap->capacity;
	}

#ifdef HMAP_THREAD_SAFE
	sem_post(&hmap->lock);
#endif
	return false;
}

#ifdef HMAP_MAKE_HASHFN

// Robert Jenkins' 32 bit integer hash function
#if __WORDSIZE == 64
uint64_t int_hash_fn(key in) {
	static uint64_t a = 0;
	a = *((uint64_t*) in);
#else
uint32_t int_hash_fn(key in) {
	static uint32_t a = 0;
	a = *((uint32_t*) in);
#endif

	a = (a+0x7ed55d16) + (a<<12);
	a = (a^0xc761c23c) ^ (a>>19);
	a = (a+0x165667b1) + (a<<5);
	a = (a+0xd3a2646c) ^ (a<<9);
	a = (a+0xfd7046c5) + (a<<3);
	a = (a^0xb55a4f09) ^ (a>>16);
	
	return a;
}

#if __WORDSIZE == 64
uint64_t node_hash_fn(key in) {
	return (uint64_t)in;
}
#else
uint32_t node_hash_fn(key in) {
	return (uint32_t)in;
}
#endif

#if __WORDSIZE == 64
uint64_t str_hash_fn(const key string) {
	uint64_t hash = 0;
#else
uint32_t str_hash_fn(const key string) {
	uint32_t hash = 0;
#endif
	char* str = (char*)string;
	int c;

    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

bool int_eq_fn(key a, key b) {
#if __WORDSIZE == 64
	return ((uint64_t) a) == ((uint64_t) b) ? true : false;
#else
	return ((uint32_t) a) == ((uint32_t) b) ? true : false;
#endif

}

bool str_eq_fn(key a, key b) {
	return (strcmp((const char*) a, (const char*)b) == 0);
}
#endif

//#include <stdio.h>
//int main(int argc, char *argv[]) {
//	hashmap *hm_teste = NULL;
//	char char_teste[15];
//	int i;
//
//	hm_teste = mk_hmap(str_hash_fn, int_eq_fn, NULL);
//
//	for (i = 1; i < 100; ++i) {
//		sprintf(char_teste, "teste-%d",i);
//		if (!hmap_add(hm_teste, char_teste, i)) {
//			printf("Error, no memory...\n");
//		} else {
//			printf("Added data %d mapped by %s...\n", i, char_teste);
//		}
//	}
//
//	for (i = 1; i < 100; ++i) {
//		sprintf(char_teste, "teste-%d",i);
//		if (hmap_get(hm_teste, char_teste) == NULL) {
//			printf("Map %s was not found\n", char_teste);
//		} else {
//			printf("Map %s was found\n", char_teste);
//		}
//	}
//
//	printf("Finalizing!\n");
//	/* Delete hashmap apps_interests */
//	if (hm_teste != NULL) {
//		free_hmap(hm_teste);
//	}
//	printf("Finalized!\n");
//	return 0;
//}
