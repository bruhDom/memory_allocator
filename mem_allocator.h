#ifndef MEM_ALLOCATOR_H
#define MEM_ALLOCATOR_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct mem_block {
    char name[8];
    size_t size;
    struct mem_block *next_pointer;
    struct mem_block *prev_pointer;
    int is_free;
} mem_block;

typedef struct free_list {
    struct free_list *prev_free;
    struct free_list *next_free;
} free_list;

typedef enum fsm_policies {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} fsm_policies;


void *mem_alloc(size_t mem_block_size);
void my_free(void *memory);
void malloc_print();
void malloc_setfsm(fsm_policies policy);
void *split_blocks(mem_block *header, size_t required_size);
void coalesce_blocks(mem_block *header);
void *reallocate(void* memory, size_t new_size);

#endif