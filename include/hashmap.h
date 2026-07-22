#pragma once 

#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

static enum{
    FNV_OFFSET_BASIS_32 = 2166136261, 
    FNV_PRIME_32        = 16777619,
    BASE_BUCKETS_SIZE   = 16
};

typedef struct Pair Pair;
static struct Pair{
    size_t key_size;
    size_t data_size;
    Pair* next;
    void* key;
    void* data;
};

typedef struct HashMap HashMap;
struct HashMap{
    size_t size;
    size_t capacity;
    Pair *buckets;
};

static uint32_t fnv1_32_hash(const void* data, const size_t data_size);

static size_t hash_function(const uint32_t hash_code, const size_t hash_map_size);

HashMap* hashmap_construct();

void hashmap_destroy(HashMap* map);

static HashMap* hashmap_rehash(HashMap* map, const size_t new_capacity);

int hashmap_add(HashMap** map, const void* key, const void* data, const size_t key_size, const size_t data_size);

int hashmap_remove(HashMap** map, const void* key, const size_t key_size);

void* hashmap_get(const HashMap* map, const void* key, const size_t key_size);

int hashmap_contains(const HashMap* map, const void* key, const size_t key_size);
