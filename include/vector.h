#pragma once 

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

static const int VEC_BASE_SIZE = 16;
             
typedef struct Vector Vector;
struct Vector{
    size_t capacity;
    size_t size;
    size_t data_type_size;
    void* vector;
};

Vector* vector_construct(size_t data_type_size);

void vector_free(Vector* vector);

static Vector* vector_resize(Vector* vector, size_t new_capacity);

int vector_append(Vector** vector, void* data);

void* vector_get(Vector* vector, size_t index);

int vector_remove(Vector** vector, size_t index);
