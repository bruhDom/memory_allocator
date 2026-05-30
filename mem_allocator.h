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



void *mem_alloc(size_t mem_block_size);
void free(void *memory);

#endif