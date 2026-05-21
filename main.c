#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pageSize.h"
#include "mem_allocator.h"


int main() {

    int page_size = init_page_size(); //tells us the size of a page on our system;

    void *memory = mem_alloc(page_size);
    printf("Memory block starting at: %p\n", memory);
    memset(memory, 0xAA, page_size - sizeof(mem_block));
    // printf("0x%02X\n", ((uint8_t*)memory)[0]);
    // printf("sbrk(0): %p\n", sbrk(0));

    free(memory);
    printf("memory freed\n");

}