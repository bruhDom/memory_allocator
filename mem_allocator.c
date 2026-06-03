#include "mem_allocator.h"
#include "pageSize.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>

static mem_block *block_head = NULL;
static mem_block *block_tail = NULL;
static free_list *free_head = NULL;


void *mem_alloc(size_t mem_block_size) {

    size_t size = mem_block_size < 48 ? 48 : mem_block_size;
    size = (size + 15) & ~(0xF); // rounds to nearest multiple of 16
    
    size_t page_size = init_page_size();
    size_t requested_size = size + sizeof(mem_block);
    size_t allocated_size = (requested_size + page_size - 1) & ~(page_size - 1);


    if(block_head != NULL) {

        free_list *block = free_head;
        while(block != NULL) {

            mem_block *header = (mem_block*)((char*)block - sizeof(mem_block)); // we convert to char* first as this is 1 byte. So when we do pointer arithmetic the sizeof type doesn't screw us over lol

            if(header->size >= allocated_size) { // If we have an already allocated block larger than what is being asked for, give them that block back
                header->is_free = 0;
                if(block->prev_free != NULL) { // If it is in the middle of a chain, we want the previous node to the point to node after current
                    (block->prev_free)->next_free = block->next_free;
                } else { 
                    free_head = block->next_free;
                }
                if(block->next_free != NULL) { // Also again if it is in middle of node we can make next block point to current node's previous
                    (block->next_free)->prev_free = block->prev_free;
                }
                // return the block

                return (void*)block;
            }

            block = block->next_free; 
        }
    }

    void *block = sbrk(allocated_size);
    if(block == (void*) - 1)
        return NULL; // that just means that we've tried to allocate a region we are not supposed to access

    // Now we need to break down block so we give user exact amount of memory, and then mark the remaining leftover memory as free.

    size_t leftover_region = allocated_size - requested_size;
    size_t minimum_required_region = sizeof(mem_block) + sizeof(free_list);

    mem_block *memory = (mem_block*)block;
    memory->size = requested_size;
    memory->is_free = 0;
    memory->next_pointer = NULL;

    // first, move to mem address for *after* header + data

    if(leftover_region >= minimum_required_region) {
        mem_block *leftover_region_header = (mem_block*)((char*)block + requested_size); // This gets us to start of new block
        leftover_region_header->is_free = 1;
        leftover_region_header->size = leftover_region;
        
        // Add the links between nodes. Leftover is now the new tail and the allocated block must point to leftover block
        leftover_region_header->prev_pointer = memory;
        memory->next_pointer = leftover_region_header;
        block_tail = leftover_region_header;
        leftover_region_header->next_pointer = NULL;

        // Now we add leftover block to the free list

        free_list* freed_leftover = (free_list*)((char*)leftover_region_header + sizeof(mem_block)); 

        if(free_head == NULL) {
            freed_leftover->next_free = NULL;
            free_head = freed_leftover;
        } else {
            free_head->prev_free = freed_leftover;
            freed_leftover->next_free = free_head;
            free_head = freed_leftover;
        }

        free_head->prev_free = NULL;
    }



    if(block_head == NULL) { // If we have no head, then this memory block is the start of our list. Nothing comes before or after it
        block_head = memory;
        block_tail = memory;
        memory->prev_pointer = NULL;
    }
    else { // If block is not start of the list, then the tail gets shifted as we add a new block onto the end of the list.
        memory->prev_pointer = block_tail;
        block_tail->next_pointer = memory;
        block_tail = memory;
    }

    void *allocated_region = block + sizeof(mem_block); // we want to return the pointer AFTER the header.

    return allocated_region;
}

void free(void *memory) {
    // If we free something, we store the pointer at the start of the region, since we can just overwrite the pointers when we request more memory

    void* start_of_block = memory - sizeof(mem_block);
    mem_block *free_memory = (mem_block*)start_of_block;
    free_memory->is_free = 1;

    free_list* memory_block = (free_list*)memory; 

    if(free_head == NULL) { // If the list is empty, start the free list at the first area of data we can write to. Nothing comes after the intial block, so that's NULL
        memory_block->next_free = NULL; 
        free_head = memory_block;
    }
    else { // If the list isn't empty, then we need to add on the free block after the head
        free_head->prev_free = memory_block;
        memory_block->next_free = free_head;
        free_head = memory_block;
    }

    free_head->prev_free = NULL;


    printf("size of mem block: %ld\n", sizeof(mem_block));
    
}

void malloc_print() { // Function to walk the block list and print out each block and their information
    
    static uint16_t i = 1;

    const char* free_states[] = {"USED", "FREE"}; 

    printf("\n ...beep boop... -- [CURRENT MEMORY STATE] -- ...beep boop...\n\n");

    if(block_head != NULL) {

        mem_block* block = block_head;

        while(block != NULL) {

            printf("[BLOCK %p - %p] %ld [%s]\n", (void*)block, (void*)((char*)block + block->size), block->size, free_states[block->is_free]);
            block = block->next_pointer;
        }
    }

}
