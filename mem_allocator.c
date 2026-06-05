#include "mem_allocator.h"
#include "pageSize.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

static mem_block *block_head = NULL;
static mem_block *block_tail = NULL;
static free_list *free_head = NULL;

static fsm_policies current_policy = FIRST_FIT;
static const size_t MIN_BLOCK_SIZE = sizeof(mem_block) + sizeof(free_list);

void *mem_alloc(size_t mem_block_size) {

    size_t size = mem_block_size < 48 ? 48 : mem_block_size;
    size = (size + 15) & ~(0xF); // rounds to nearest multiple of 16
    
    size_t page_size = init_page_size();
    size_t requested_size = size + sizeof(mem_block);
    size_t allocated_size = (requested_size + page_size - 1) & ~(page_size - 1);


    switch (current_policy) {
        case FIRST_FIT:
        
            if(block_head != NULL) {
                free_list *block = free_head;

                while(block != NULL) {
                    mem_block *header = (mem_block*)((char*)block - sizeof(mem_block)); // we convert to char* first as this is 1 byte. So when we do pointer arithmetic the sizeof type doesn't screw us over lol

                    if(header->size >= requested_size) { // If we have an already allocated block larger than what is being asked for, give them that block back
                        header->is_free = 0;
                        
                        if(block->prev_free != NULL) { // If it is in the middle of a chain, we want the previous node to the point to node after current
                            (block->prev_free)->next_free = block->next_free;
                        } 
                        else { 
                            free_head = block->next_free;
                        }
                        if(block->next_free != NULL) { // Also again if it is in middle of node we can make next block point to current node's previous
                            (block->next_free)->prev_free = block->prev_free;
                        }
                        // return the block
                        
                        split_blocks(header, requested_size); // splits blocks and carves out leftover. Put into function cause its cleaner and i had issues before.

                        return (void*)block;
                    }

                    block = block->next_free; 
                }
            }
        break;
        case BEST_FIT:
            size_t best_size = SIZE_MAX;
            

            if(block_head != NULL) {
                free_list *block = free_head;
                free_list *best_block = NULL;

                while(block != NULL) {
                    
                    mem_block *header = (mem_block*)((char*)block - sizeof(mem_block));

                    if(header->size >= requested_size) {
                        if(header->size - requested_size < best_size) {
                            best_size = header->size - requested_size;
                            best_block = block;
                        }

                    }
                    block = block->next_free;

                }
                if(best_block != NULL) {
                    if(best_block->prev_free != NULL) {
                        (best_block->prev_free)->next_free = best_block->next_free;
                    }
                    else {
                        free_head = best_block->next_free;
                    }
                    if(best_block->next_free != NULL) {
                        (best_block->next_free)->prev_free = best_block->prev_free;
                    }

                    
                    mem_block* header = (mem_block*)((char*)best_block - sizeof(mem_block));
                    header->is_free = 0;
                    split_blocks(header, requested_size); // splits blocks and carves out leftover. Put into function cause its cleaner and i had issues before.

                    return (void*)best_block;
                }
            }

        break;
        case WORST_FIT:
            size_t worst_size = 0;

            if(block_head != NULL) {
                free_list* block = free_head;
                free_list* worst_block = block;

                while(block != NULL) {
                    mem_block* header = (mem_block*)((char*)block - sizeof(mem_block));

                    if(header->size >= requested_size) {
                        if(header->size > worst_size) {
                            worst_size = header->size;
                            worst_block = block;
                        }
                    }

                    block = block->next_free;
                }
                if(worst_block != NULL) {
                    if(worst_block->prev_free != NULL) {
                        (worst_block->prev_free)->next_free = worst_block->next_free;
                    } 
                    else {
                        free_head = worst_block->next_free;
                    }
                    if(worst_block->next_free != NULL) {
                        (worst_block->next_free)->prev_free = worst_block->prev_free;
                    }
                    
                    mem_block* header = (mem_block*)((char*)worst_block - sizeof(mem_block));
                    header->is_free = 0;
                    split_blocks(header, requested_size); // splits blocks and carves out leftover. Put into function cause its cleaner and i had issues before.

                    return (void*)worst_block;
                }

            }
        break;      
    }
    
    void *block = sbrk(allocated_size);
    if(block == (void*) - 1)
        return NULL; // that just means that we've tried to allocate a region we are not supposed to access

    // Now we need to break down block so we give user exact amount of memory, and then mark the remaining leftover memory as free.

    mem_block *memory = (mem_block*)block;
    memory->size = allocated_size;
    memory->is_free = 0;
    memory->next_pointer = NULL;

    // first, move to mem address for *after* header + data

    

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

    split_blocks(memory, requested_size); // splits blocks and carves out leftover. Put into function cause its cleaner and i had issues before.
   

    void *allocated_region = block + sizeof(mem_block); // we want to return the pointer AFTER the header.

    return allocated_region;
}

void my_free(void *memory) {
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

    coalesce_blocks(free_memory);
}

void malloc_print() { // Function to walk the block list and print out each block and their information
    
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

void malloc_setfsm(fsm_policies policy) {
    current_policy = policy;
}

void *split_blocks(mem_block* header, size_t required_size) { // 
    
    size_t leftover_region = header->size - required_size;  

    if(leftover_region >= MIN_BLOCK_SIZE) { // Make sure that the region is big enough such that it can store a header
        mem_block *leftover_region_header = (mem_block*)((char*)header + required_size); // This gets us to start of new block
        leftover_region_header->is_free = 1;
        leftover_region_header->size = leftover_region;
        header->size = required_size;
        
        // Add the links between nodes. Leftover is now the new tail and the allocated block must point to leftover block
        mem_block* temp_check = header->next_pointer;
        if(temp_check != NULL) { // If the header is not at the tail, then we want to append the leftover block into block list
            leftover_region_header->prev_pointer = header;
            header->next_pointer = leftover_region_header;
            leftover_region_header->next_pointer = temp_check;
            temp_check->prev_pointer = leftover_region_header;
        }
        else { // If it is the tail, then the new leftover is now the tail
            leftover_region_header->prev_pointer = header;
            header->next_pointer = leftover_region_header;
            block_tail = leftover_region_header;
        }

        // Now we add leftover block to the free list

        free_list* freed_leftover = (free_list*)((char*)leftover_region_header + sizeof(mem_block)); 

        // Add to free list
        if(free_head == NULL) {
            freed_leftover->next_free = NULL;
            free_head = freed_leftover;
        } else {
            free_head->prev_free = freed_leftover;
            freed_leftover->next_free = free_head;
            free_head = freed_leftover;
        }

        return (void*)leftover_region_header;
    }

    return NULL;
}

void coalesce_blocks(mem_block *header) {

    mem_block* end_block = header;

    if(header->next_pointer != NULL) { // IM GOING INSANE WITH MEMORY AJSKDJHASIJDHASOINDNASOIDN
        if(header->next_pointer->is_free) {
            header->size += header->next_pointer->size;

            mem_block *coalesced_block = header->next_pointer; // So we add blocks together, then remove the adjacent block from block list
            if(coalesced_block->next_pointer != NULL) {
                header->next_pointer = coalesced_block->next_pointer;
                (coalesced_block->next_pointer)->prev_pointer = header;
            }
            else {
                header->next_pointer = NULL;
                block_tail = header;    
            }

            free_list* old_block = (free_list*)((char*)coalesced_block + sizeof(mem_block)); // Also remove free adjacent block from free list

            if(old_block->next_free != NULL) {
                (old_block->next_free)->prev_free = old_block->prev_free;
            }
            if(old_block->prev_free != NULL) {
                (old_block->prev_free)->next_free = old_block->next_free;
            }
            else {
                free_head = old_block->next_free;
            }

        }
    }
    if(header->prev_pointer != NULL) { // If we have a free block before it, then the current block merges INTO that block
        if(header->prev_pointer->is_free) {
            header->prev_pointer->size += header->size;
            end_block = header->prev_pointer;

            mem_block* new_block = header->prev_pointer;

            if(header->next_pointer != NULL) {
                new_block->next_pointer = header->next_pointer;
                (header->next_pointer)->prev_pointer = new_block;
            }
            else {
                new_block->next_pointer = NULL;
                block_tail = new_block;
            }

            free_list* old_block = (free_list*)((char*)header + sizeof(mem_block)); // Again remove current block from free list

            if(old_block->next_free != NULL) {
                (old_block->next_free)->prev_free = old_block->prev_free;
            }
            if(old_block->prev_free != NULL) {
                (old_block->prev_free)->next_free = old_block->next_free;
            }
            else {
                free_head = old_block->next_free;
            }
        }
    }

    size_t remaining_memory = end_block->size % init_page_size(); // Now we need to return to the kernel, so integer division to tell us how many pages we can return

    if(end_block->next_pointer == NULL && (remaining_memory == 0 || remaining_memory >= MIN_BLOCK_SIZE)) { // Must ensure that we are at end of chain and that it is safe to return block

        if(remaining_memory >= MIN_BLOCK_SIZE) {
            end_block->size = remaining_memory; // If its bigger, then we just set the size to remaining_memory since the block still remains
        }
        else if(remaining_memory == 0) {  // If its 0, then we are removing the block entirely

            if(end_block->prev_pointer != NULL) {
                (end_block->prev_pointer)->next_pointer = NULL;
                block_tail = end_block->prev_pointer;
            }
            else {
                block_head = NULL;
                block_tail = NULL;
            }
            free_list *old_block = (free_list*)((char*)end_block + sizeof(mem_block));

            if(old_block->next_free != NULL) {
                (old_block->next_free)->prev_free = old_block->prev_free;
            }
            if(old_block->prev_free != NULL) {
                (old_block->prev_free)->next_free = old_block->next_free;
            }
            else {
                free_head = old_block->next_free;
            }
            
            size_t amount_to_return = (end_block->size / init_page_size()) * init_page_size(); // Now return a multiple of page size to kernel
            sbrk(-(amount_to_return));

        }   
    }
}

void *reallocate(void* memory, size_t new_size) {

    if(memory == NULL) {
        return mem_alloc(new_size);
    }
    if(new_size == 0) {
        my_free(memory);
        return NULL;
    }

    mem_block *header = (mem_block*)((char*)memory - sizeof(mem_block));
    size_t block_size = header->size;

    size_t requested_size = new_size + sizeof(mem_block); // add on header as size requested does not include this.

    if(requested_size <= block_size) { // If the size they want is smaller than or equal to the current block
        mem_block* leftover = (mem_block*)split_blocks(header, requested_size);
        if(leftover != NULL) coalesce_blocks(leftover);
        return memory;
    }
    else { // Otherwise the block must be larger than the current size of the block

        if(header->next_pointer != NULL && header->next_pointer->is_free) { // First check we have an adjacent block we can grow into if we are in middle of list
            if(header->next_pointer->size + header->size >= requested_size) { // Next check that the sizes of these blocks are large enough to hold reallocation
               
                header->size += header->next_pointer->size; // extend intial block

                // Fix block list now
                mem_block* merged_block = header->next_pointer;
                if(merged_block->next_pointer != NULL) { // If we are still in middle of list
                    (merged_block->next_pointer)->prev_pointer = header;
                    header->next_pointer = merged_block->next_pointer;
                }
                else {
                    block_tail = header;
                    header->next_pointer = NULL;
                }

                free_list* freed_block = (free_list*)((char*)merged_block + sizeof(mem_block)); // Now remove coalesced block from free list

                if(freed_block->next_free != NULL) {
                    (freed_block->next_free)->prev_free = freed_block->prev_free;
                }
                if(freed_block->prev_free != NULL) {
                    (freed_block->prev_free)->next_free = freed_block->next_free;
                }
                else {
                    free_head = freed_block->next_free;
                }

                split_blocks(header, requested_size); // Once we have removed from free_list, we split blocks into any leftover and then return the memory we reallocated
                return memory;

            }
            else { // In the case where the adjacent block can't hold the memory, we then just allocate at the end
                void* new_memory = mem_alloc(new_size);
                size_t new_block_size = header->size - sizeof(mem_block); // Have to allocate them an amount *smaller* than the new block to ensure we dont have some memory leak
                memcpy(new_memory, memory, new_block_size);
                my_free(memory);
                return new_memory;
            }
        }
        void* new_memory = mem_alloc(new_size);
        return new_memory;
    }
}