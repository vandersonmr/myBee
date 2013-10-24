/*
 * hashmap.h 
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

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifndef HASHMAP_H_
#define HASHMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#define HMAP_PRESET_SIZE 	(2 << 6) // use a power of 2 for faster array access
#define HMAP_GROWTH_RATE 	2
#define HMAP_MAKE_HASHFN
#define HMAP_THREAD_SAFE 	// build with -lrt
#define HMAP_DESTRUCTORS

#ifdef HMAP_THREAD_SAFE
	#include <semaphore.h>
#endif

typedef void* key;
typedef void* val;

struct key_val_map {
	key k; // key
	val v; // value
#if __WORDSIZE == 64
	uint64_t h; // hash
#else
	uint32_t h; // hash
#endif
};

struct hashmap {
	struct key_val_map *map;
	uint32_t size;
	uint32_t capacity;
#if __WORDSIZE == 64
	uint64_t (*hash_fn)(key);
#else
	uint32_t (*hash_fn)(key);
#endif
	bool (*eq_fn)(key, key);
#ifdef HMAP_DESTRUCTORS
	void (*del_fn)(val);
#endif
#ifdef HMAP_THREAD_SAFE
	sem_t lock;
#endif
};


typedef struct key_val_map key_val_map;
typedef struct hashmap hashmap;

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
    );
			
void free_hmap(hashmap*);

bool __hmap_add(hashmap* hmap, const key in, val out);
#define hmap_add(hmap, in, out) __hmap_add(hmap, (key) in, (val) out)

val __hmap_get(hashmap* hmap, const key in);
#define hmap_get(hmap, in) __hmap_get(hmap, (key) in)

bool __hmap_set(hashmap* hmap, const key in, val out);
#define hmap_set(hmap, in, out) __hmap_set(hmap, (key) in, (val) out)

key_val_map __hmap_remove(hashmap* hmap, const key in);
#define hmap_remove(hmap, in) __hmap_remove(hmap, (key) in)

#if __WORDSIZE == 64
extern uint64_t int_hash_fn(key in);
extern uint64_t node_hash_fn(key in);
extern uint64_t str_hash_fn(key in);
#else
extern uint32_t int_hash_fn(key in);
extern uint32_t node_hash_fn(key in);
extern uint32_t str_hash_fn(key in);
#endif

extern bool int_eq_fn(key a, key b);
extern bool str_eq_fn(key a, key b);

#ifdef __cplusplus
}
#endif
#endif
