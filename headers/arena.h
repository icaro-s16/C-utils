#pragma once 

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "vector.h"

enum{
    SINGLE_PAGE = 0b0001,
    SHARED = 0b0010
};

typedef unsigned char byte;

typedef struct Node Node;
static struct Node{
    void* data;
    struct Node* next;
    struct Node* prev;
};

typedef struct ArenaPage ArenaPage;
static struct ArenaPage{
    size_t offset;
    byte* chunck;
};

typedef ArenaAllocation;
static struct ArenaAllocation{
    unsigned current_page;
    size_t size;
    byte* address;
};

typedef struct Arena Arena;
typedef struct Arena{
    unsigned flags;
    size_t page_size;
    size_t n_pages;
    Vector *allocations; 
    Node* head;
    Node* tail;
};

Arena* arena_construct(size_t page_size, unsigned flags);

void arena_destroy(Arena* arena);

static void arena_push_page(Arena** arena);

void* arena_malloc(Arena* arena, size_t data_size);

void* arena_realloc(Arena* arena, void* address, size_t new_size);

static void arena_free_pages(Arena* arena);
