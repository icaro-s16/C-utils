#pragma once 

#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define FNV_OFFSET_BASIS_32 2166136261 

#define FNV_PRIME_32 16777619 

#define BASE_BUCKETS_SIZE 16

typedef struct{
    size_t key_size;
    size_t data_size;
    struct Pair* next;
    void* key;
    void* data;
}Pair;

typedef struct{
    size_t size;
    size_t capacity;
    Pair *buckets;
}HashMap;

uint32_t fnv1_32_hash(const void* data, const size_t data_size);

size_t hash_function(const uint32_t hash_code, const size_t hash_map_size);

HashMap* hashmap_construct();

void hashmap_destroy(HashMap* map);

HashMap* hashmap_rehash(HashMap* map, const size_t new_capacity);

int hashmap_add(HashMap** map, const void* key, const void* data, const size_t key_size, const size_t data_size);

int hashmap_remove(HashMap** map, const void* key, const size_t key_size);

void* hashmap_get(const HashMap* map, const void* key, const size_t key_size);

int hashmap_contains(const HashMap* map, const void* key, const size_t key_size);