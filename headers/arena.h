#pragma once 

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "vector.h"

#define SINGLE_PAGE 0b0001

typedef unsigned char byte;


struct Node{
    void* data;
    struct Node* next;
    struct Node* prev;
};

typedef struct{
    size_t offset;
    byte* chunck;
}ArenaPage;

typedef struct{
    unsigned current_page;
    size_t size;
    byte* address;
}ArenaAllocation;

typedef struct{
    unsigned flags;
    size_t page_size;
    size_t n_pages;
    Vector *allocations; 
    struct Node* head;
    struct Node* tail;
}Arena;

Arena* arena_construct(size_t page_size, unsigned flags);

void arena_destroy(Arena* arena);

void arena_push_page(Arena** arena);

void* arena_malloc(Arena* arena, size_t data_size);

void* arena_realloc(Arena* arena, void* address, size_t new_size);

void arena_free_pages(Arena* arena);