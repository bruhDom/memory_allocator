#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pageSize.h"
#include "mem_allocator.h"


int main() {

    int page_size = init_page_size(); //tells us the size of a page on our system;

    
    // ------ ALLOCATE SOME MEMORY -------

    void *memory1 = mem_alloc(page_size);
    void *memory2 = mem_alloc(page_size);
    void *memory3 = mem_alloc(page_size);

    printf("\nmemory1 starts at: %p\nmemory2 starts at: %p\nmemory3 starts at: %p\n", memory1, memory2, memory3);

    // ------- WRITE TO MEMORY ---------

    void *header1 = memory1 - sizeof(mem_block);
    mem_block *headerBlock1 = (mem_block*)header1;
    
    void *header2 = memory2 - sizeof(mem_block);
    mem_block *headerBlock2 = (mem_block*)header2;

    void *header3 = memory3 - sizeof(mem_block);
    mem_block *headerBlock3 = (mem_block*)header3;

    memset(memory1, 0xAA, headerBlock1->size - sizeof(mem_block));
    memset(memory2, 0xBB, headerBlock2->size - sizeof(mem_block));
    memset(memory3, 0xCC, headerBlock3->size - sizeof(mem_block));

    printf("\nmemory1 contains: 0x%02X\n", ((uint8_t*)memory1)[0]);
    printf("\nmemory2 contains: 0x%02X\n", ((uint8_t*)memory2)[0]);
    printf("\nmemory3 contains: 0x%02X\n", ((uint8_t*)memory3)[0]);


    // ------- FREE SOME MEMORY -----------

    free(memory2);

    printf("\nmemory block 2 free status: %d", headerBlock2->is_free);

    if(headerBlock2->is_free) 
        printf("\nmemory has been freed\n");
}