#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pageSize.h"
#include "mem_allocator.h"


int main() {


    int page_size = init_page_size(); //tells us the size of a page on our system;
    malloc_setfsm(FIRST_FIT);
    void *memory1 = mem_alloc(1024);
    void *memory2 = mem_alloc(1024);
    void *memory3 = mem_alloc(1024);
    void *memory4 = mem_alloc(1024);

    malloc_print();

    // malloc_setfsm(WORST_FIT);
    // void *memory4 = mem_alloc(256);
    return 0;

}