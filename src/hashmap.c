#include "../headers/hashmap.h"

static uint32_t fnv1_32_hash(const void* data, const size_t data_size){
    assert(data != NULL);
    assert(data_size > 0);

    uint32_t hash = FNV_OFFSET_BASIS_32;

    for(unsigned offset = 0; offset < data_size; offset ++){
        hash *= FNV_PRIME_32;
        hash ^= *((uint8_t*)(data + offset));
    }

    return hash;
}


static inline size_t hash_function(const uint32_t hash_code, const size_t hash_map_size){
    double base_mul = ((1.0 + sqrt(5.0))/2.0) - 1.0;
    return (floor(hash_map_size * fmod((hash_code * base_mul), 1.0))); 
}


HashMap* hashmap_construct(){
    HashMap* map  = malloc(sizeof(HashMap));
    map->buckets  = calloc(BASE_BUCKETS_SIZE, sizeof(Pair));
    map->capacity = BASE_BUCKETS_SIZE;
    map->size     = 0;
    return map;
}

void hashmap_destroy(HashMap* map){
    assert(map != NULL);
   
    
    if (map->buckets == NULL){
        free(map);
        return;
    }

    for(int offset = 0; offset < map->capacity; offset++){
        Pair* move_pointer = (Pair*)map->buckets[offset].next;
        
        if (map->buckets[offset].data != NULL) free(map->buckets[offset].data);
        if (map->buckets[offset].key != NULL) free(map->buckets[offset].key);

        while (move_pointer != NULL)
        {
            Pair* next_pair = (Pair*)move_pointer->next;
            if (move_pointer->data != NULL) free(move_pointer->data);
            if (move_pointer->key != NULL) free(move_pointer->key);
            free(move_pointer);
            move_pointer = next_pair;
        }
        
    }
    
    free(map->buckets);
    free(map);

}

static HashMap* hashmap_rehash(HashMap* map, const size_t new_capacity){
    assert(map != NULL);
    assert(new_capacity > 0);
    
    HashMap* new_map = malloc(sizeof(HashMap));
    new_map->capacity = new_capacity;
    new_map->size = 0;
    new_map->buckets = calloc(new_map->capacity, sizeof(Pair));

    for (int offset = 0; offset < map->capacity; offset++){
        if (map->buckets[offset].key == NULL) continue;
        
        Pair* actual_pair = &map->buckets[offset];
        while (actual_pair != NULL){
            hashmap_add(&new_map, actual_pair->key, actual_pair->data, actual_pair->key_size, actual_pair->data_size);
            actual_pair = actual_pair->next;
        }

    }
    
    hashmap_destroy(map);
    return new_map;
}

void hashmap_add(HashMap** map, const void* key, const void* data, const size_t key_size, const size_t data_size){
    assert(map != NULL);
    assert(*map != NULL);
    assert(key != NULL);
    assert(data != NULL);
    assert(key_size > 0);
    assert(data_size > 0);
    
    uint32_t hash_code = fnv1_32_hash(key, key_size);

    if ((double)(*map)->size/ (double)(*map)->capacity > 0.75){
        *map = hashmap_rehash(*map, (*map)->capacity * 2);
    }
    

    size_t bucket = hash_function(hash_code, (*map)->capacity);

    if ((*map)->buckets[bucket].key == NULL){
        (*map)->size += 1;
        (*map)->buckets[bucket].data = malloc(data_size);
        memcpy((*map)->buckets[bucket].data, data, data_size);
        (*map)->buckets[bucket].data_size = data_size;
        (*map)->buckets[bucket].key = malloc(key_size);
        memcpy((*map)->buckets[bucket].key, key, key_size);
        (*map)->buckets[bucket].key_size = key_size;
        (*map)->buckets[bucket].next = NULL;
        return;
    }

    Pair* slow_pointer = NULL;
    Pair* fast_pointer = &(*map)->buckets[bucket];

    while(
        fast_pointer != NULL
    ){
        if (
            fast_pointer->key_size == key_size && 
            memcmp(fast_pointer->key, key, key_size) == 0
        ){
            if (fast_pointer->data != NULL) free(fast_pointer->data);
            fast_pointer->data = malloc(data_size);
            fast_pointer->data_size = data_size;
            memcpy(fast_pointer->data, data, data_size);
            return;
        }

        slow_pointer = fast_pointer;
        fast_pointer = fast_pointer->next;
    }

    (*map)->size += 1;
    slow_pointer->next = malloc(sizeof(Pair));
    slow_pointer->next->data = malloc(data_size);
    memcpy(slow_pointer->next->data, data, data_size);
    slow_pointer->next->data_size = data_size;
    slow_pointer->next->key = malloc(key_size);
    memcpy(slow_pointer->next->key, key, key_size);
    slow_pointer->next->key_size = key_size;
    slow_pointer->next->next = NULL;

}

void hashmap_remove(HashMap** map, const void* key, const size_t key_size){
    assert(map != NULL);
    assert(*map != NULL);
    assert(key != NULL);
    assert(key_size > 0);
    
    uint32_t hash_code = fnv1_32_hash(key, key_size);
    size_t bucket = hash_function(hash_code, (*map)->capacity);

    Pair* slow_pointer = NULL;
    Pair* fast_pointer = &(*map)->buckets[bucket];

    while(
        fast_pointer != NULL
    ){
        if (
            fast_pointer->key_size == key_size &&
            memcmp(fast_pointer->key, key, key_size) == 0
        ){
            break;
        }
        slow_pointer = fast_pointer;
        fast_pointer = fast_pointer->next;
    }

    if fast_pointer == NULL) return;

    (*map)->size -= 1;
    if (slow_pointer == NULL){
        if (fast_pointer->data != NULL) free(fast_pointer->data);
        if (fast_pointer->key != NULL) free(fast_pointer->key);
        fast_pointer->data = NULL;
        fast_pointer->key = NULL;
        fast_pointer->data_size = 0;
        fast_pointer->key_size = 0;
        if (fast_pointer->next != NULL){
            Pair* next = fast_pointer->next;
            fast_pointer->data = next->data;
            fast_pointer->key = next->key;
            fast_pointer->data_size = next->data_size;
            fast_pointer->key_size = next->key_size;
            fast_pointer->next = next->next;
            free(next);
        }
        if ((double)(*map)->size / (double)(*map)->capacity <= 0.25 && (*map)->capacity > BASE_BUCKETS_SIZE)
            *map = hashmap_rehash(*map, (*map)->capacity / 2);
        return;
    }

    slow_pointer->next = fast_pointer->next;
    if (fast_pointer->data != NULL) free(fast_pointer->data);
    if (fast_pointer->key != NULL) free(fast_pointer->key);
    free(fast_pointer);
    if ((double)(*map)->size / (double)(*map)->capacity <= 0.25 && (*map)->capacity > BASE_BUCKETS_SIZE)
        *map = hashmap_rehash(*map, (*map)->capacity / 2);
    return;
}

void* hashmap_get(const HashMap* map, const void* key, const size_t key_size){
    assert(map != NULL);
    assert(key != NULL);
    assert(key_size > 0);
    
    uint32_t hash_code = fnv1_32_hash(key, key_size);
    size_t bucket = hash_function(hash_code, map->capacity);

    Pair* move_pointer = &map->buckets[bucket];

    while (
        move_pointer != NULL
    ){
        if (
            move_pointer->key_size == key_size && 
            memcmp(move_pointer->key, key, key_size) == 0
        ){
            return move_pointer->data;
        }
        move_pointer = move_pointer->next;
    }

    return NULL;
}

int hashmap_contains(const HashMap* map, const void* key, const size_t key_size){
    assert(map != NULL);
    assert(key != NULL);
    assert(key_size > 0);

    uint32_t hash_code = fnv1_32_hash(key, key_size);
    size_t bucket = hash_function(hash_code, map->capacity);

    Pair* actual_pair = &map->buckets[bucket];

    if (actual_pair->key == NULL) return 0;

    while(actual_pair != NULL){
        
        if (
            actual_pair->key_size == key_size && 
            memcmp(actual_pair->key, key, key_size) == 0
        ) return 1;

        actual_pair = actual_pair->next;
    }

    return 0;
}

