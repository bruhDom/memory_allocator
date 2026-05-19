#include <stdio.h>
#include "pageSize.h"
#include "mem_allocator.h"

int main() {

    init_page_size(); //tells us the size of a page on our system;

    void *memory = mem_alloc(4096);

    printf("Memory block starting at: %p\n", memory);

}