#include "../headers/vector.h"

Vector* vector_construct(size_t data_type_size){
    Vector* vec = malloc(sizeof(Vector));
    vec->data_type_size = data_type_size;
    vec->capacity = VEC_BASE_SIZE;
    vec->size = 0;
    vec->vector = calloc(data_type_size, vec->capacity);
    return vec;
}

void vector_free(Vector* vector){
    if (vector == NULL) {
        fprintf(stderr, "\033[31mFatal Error: Trying to free a NULL vector\033[0m\n");
        abort(); 
    }
    if (vector->vector != NULL ) free(vector->vector);
    free(vector);
}

Vector* vector_resize(Vector* vector, size_t new_capacity){
    if (vector == NULL) return NULL;

    vector->capacity = new_capacity;
    void *new_vec = (void*)reallocarray(vector->vector, vector->capacity, vector->data_type_size);
    if (new_vec == NULL) {
        fprintf(stderr, "\033[31mFatal Error: The program runs out of memory to resize the vector\033[0m\n");
        abort();
    }
    vector->vector = new_vec;
    return vector;
}

int vector_append(Vector** vector, void* data){
    if ((*vector) == NULL) return -1;
    (*vector)->size += 1;
    if ((*vector)->size >= (*vector)->capacity) *vector = vector_resize(*vector, (*vector)->capacity * 2);
    memcpy(
        (*vector)->vector + (((*vector)->size - 1) * (*vector)->data_type_size), 
        data, (*vector)->data_type_size
    );
    return 0;
}

void* vector_get(Vector* vector, size_t index){
    if (vector == NULL) {
        fprintf(stderr, "\033[31mFatal Error: The vector is point to NULL pointer\033[0m\n");
        abort();
    }

    if (index >= vector->size){
        fprintf(stderr, "\033[31mFatal Error: The index is out of range for the vector\033[0m\n");
        abort();
    }

    return (vector->vector + (index * vector->data_type_size));
}

int vector_remove(Vector** vector, size_t index){
    if ((*vector) == NULL) return -1;

    if (index >= (*vector)->size) return -1;

    if ( index != ((*vector)->size - 1))
        memmove(
            (*vector)->vector + (index * (*vector)->data_type_size), 
            (*vector)->vector + ((index + 1) * (*vector)->data_type_size), 
            (((*vector)->size - 1) - index) * (*vector)->data_type_size
        );
    
    (*vector)->size -= 1;
    if ((*vector)->size < (*vector)->capacity * 0.25 && (*vector)->capacity > VEC_BASE_SIZE ) {
        *vector = vector_resize(*vector, (size_t)(*vector)->capacity / 2);
    }
    
    return 0;
}

