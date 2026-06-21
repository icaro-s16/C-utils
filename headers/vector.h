#pragma once 

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#define VEC_BASE_SIZE 16
             

typedef struct{
    size_t capacity;
    size_t size;
    size_t data_type_size;
    void* vector;
}Vector;

Vector* vector_construct(size_t data_type_size);

void vector_free(Vector* vector);

Vector* vector_resize(Vector* vector, size_t new_capacity);

int vector_append(Vector** vector, void* data);

void* vector_get(Vector* vector, size_t index);

int vector_remove(Vector** vector, size_t index);
