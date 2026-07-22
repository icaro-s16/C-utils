#include "../headers/arena.h"

Arena* arena_construct(size_t page_size, unsigned flags){
    assert(page_size > 0);
    
    Arena* arena = malloc(sizeof(Arena));
    arena->flags = flags;
    arena->page_size = page_size;
    arena->n_pages = 1;
    arena->allocations = vector_construct(sizeof(ArenaAllocation));
    arena->head = malloc(sizeof(struct Node));
    arena->tail = arena->head;
    arena->head->next = NULL;
    arena->head->prev = NULL;
    (arena->head->data) = malloc(sizeof(ArenaPage));
    ((ArenaPage*) arena->head->data)->chunck = calloc(sizeof(byte), arena->page_size);
    ((ArenaPage*) arena->head->data)->offset = 0;
    return arena;
}

void arena_destroy(Arena* arena){
    assert(arena != NULL);
    
    vector_free(arena->allocations);
    while (arena->head != NULL){
        struct Node* current_node = arena->head;
        arena->head = arena->head->next;
        free(((ArenaPage*)current_node->data)->chunck);
        free(current_node->data);
        free(current_node);
    }
    free(arena);
}


static void arena_push_page(Arena** arena){
    assert((*arena)->flags & SINGLE_PAGE == 0);
    assert(arena != NULL);
    assert(*arena != NULL);
    
    (*arena)->n_pages += 1;
    
    if ((*arena)->head == NULL){
        (*arena)->head = malloc(sizeof(struct Node));
        (*arena)->tail = (*arena)->head;
        (*arena)->head->next = NULL;
        (*arena)->head->prev = NULL;
        (*arena)->head->data = malloc(sizeof(ArenaPage));
        ((ArenaPage*) (*arena)->head->data)->chunck = calloc(sizeof(byte), (*arena)->page_size);
        ((ArenaPage*) (*arena)->head->data)->offset = 0;
        return;
    }

    (*arena)->tail->next = malloc(sizeof(struct Node));
    (*arena)->tail->next->prev = (*arena)->tail;
    (*arena)->tail = (*arena)->tail->next;
    (*arena)->tail->next = NULL;
    (*arena)->tail->data = malloc(sizeof(ArenaPage));
    ((ArenaPage*) (*arena)->tail->data)->chunck = calloc(sizeof(byte), (*arena)->page_size);
    ((ArenaPage*) (*arena)->tail->data)->offset = 0;
}

void* arena_malloc(Arena* arena, size_t data_size){
    assert(arena != NULL);
    assert(data_size > 0);

    Node* current_node = arena->head;
    unsigned n_actual_page = 0;

    while (current_node != NULL && ((ArenaPage*)current_node->data)->offset + data_size > arena->page_size){
        current_node = current_node->next;
        n_actual_page += 1;
    }

    if (current_node == NULL){
        arena_push_page(&arena);
        current_node = arena->tail;
    }

    byte *alloc_space = (((ArenaPage*)current_node->data)->chunck + ((ArenaPage*)current_node->data)->offset);
    
    ((ArenaPage*)current_node->data)->offset += data_size;

    ArenaAllocation allocation_info = {
        n_actual_page,
        data_size,
        (byte*)alloc_space
    };

    vector_append(&arena->allocations, &allocation_info);

    return alloc_space;
}

void* arena_realloc(Arena* arena, void* address, size_t new_size){
    assert(arena != NULL);
    assert(address != NULL);
    assert(new_size > 0);
    
    ArenaAllocation* allocation_info = NULL;

    for (int i = 0; i < arena->allocations->size; i ++){
        if ((allocation_info = vector_get(arena->allocations, i)) == NULL) {
            fprintf(stderr, "\033[31mFatal Error: The index is out of range for the vector\033[0m\n");
            abort();
        }

        if (allocation_info->address == address)
            break;
        
        allocation_info = NULL;
    }

    assert(allocation_info != NULL);

    if (allocation_info->size >= new_size) return address;

    Node* current_node = arena->head;
    for (int i = 0; i < allocation_info->current_page; i ++)
        current_node = current_node->next;

    if (
        allocation_info->address + allocation_info->size  ==  ((ArenaPage*)current_node->data)->chunck + ((ArenaPage*)current_node->data)->offset
    ){
        ((ArenaPage*)current_node->data)->offset -= allocation_info->size;
        ((ArenaPage*)current_node->data)->offset += new_size;
        return address;
    }

    current_node = arena->head;
    unsigned n_actual_page = 0;

    while (current_node != NULL && ((ArenaPage*)current_node->data)->offset + new_size > arena->page_size){
        current_node = current_node->next;
        n_actual_page += 1;
    }

    if (current_node == NULL){
        arena_push_page(&arena);
        current_node = arena->tail;
    }

    allocation_info->address = (void*)(((ArenaPage*)current_node->data)->chunck + ((ArenaPage*)current_node->data)->offset);
    
    ((ArenaPage*)current_node->data)->offset += new_size;
    
    memcpy(
        allocation_info->address,
        address, 
        allocation_info->size
    );
    
    allocation_info->size = new_size;
    allocation_info->current_page = n_actual_page;
    
    return address;
}

static void arena_free_pages(Arena* arena){
    assert(arena != NULL);
    Node* current_node = arena->head;
    while (current_node != NULL){
        ((ArenaPage*)current_node->data)->offset = 0;
        current_node = current_node->next;
    }
}
