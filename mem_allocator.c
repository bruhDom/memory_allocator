#include "mem_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *mem_alloc(size_t mem_block_size) {

    mem_block memory;

    memory.size = mem_block_size;

    void *block = sbrk(memory.size);
    void *allocated_region = block + sizeof(memory); // we want to return the pointer AFTER the header.

    return allocated_region;
}
