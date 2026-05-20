#include "mem_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void *mem_alloc(size_t mem_block_size) {

    size_t size = mem_block_size < 48 ? 48 : mem_block_size;

    size = (size + 15) & ~(0xF); // rounds to nearest multiple of 16
    printf("%ld\n", size);

    size_t allocated_size = size + sizeof(mem_block);

    allocated_size = (allocated_size + 4095) & ~(0xFFF);
    printf("%ld\n", allocated_size);

    void *block = sbrk(allocated_size);

    mem_block *memory = (mem_block*)block;
    memory->size = allocated_size;
    memory->prev_pointer = NULL;
    memory->next_pointer = NULL;

    void *allocated_region = block + sizeof(mem_block); // we want to return the pointer AFTER the header.

    return allocated_region;
}

void free(void *memory) {

}
