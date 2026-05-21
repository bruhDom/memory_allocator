#include "mem_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void *mem_alloc(size_t mem_block_size) {

    size_t size = mem_block_size < 48 ? 48 : mem_block_size;
    size = (size + 15) & ~(0xF); // rounds to nearest multiple of 16

    size_t allocated_size = size + sizeof(mem_block);
    allocated_size = (allocated_size + 4095) & ~(0xFFF);

    void *block = sbrk(allocated_size);
    if(block == (void*) - 1)
        return NULL; // that jsut means that we've tried to allocate a region we are not supposed to access

    mem_block *memory = (mem_block*)block;
    memory->size = allocated_size;
    memory->prev_pointer = NULL;
    memory->next_pointer = NULL;

    void *allocated_region = block + sizeof(mem_block); // we want to return the pointer AFTER the header.

    return allocated_region;
}

void free(void *memory) {
    // If we free something, we store the pointer at the start of the region, since we can just overwrite the pointers when we request more memory

    void* start_of_block = memory - sizeof(mem_block);
    mem_block *free_memory = (mem_block*)start_of_block;
    free_memory->is_free = 1;
    printf("%p\n", free_memory);
    printf("%d\n", free_memory->is_free);
    printf("size of mem block: %ld\n", sizeof(mem_block));



    
}
