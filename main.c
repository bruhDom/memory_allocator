#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pageSize.h"
#include "mem_allocator.h"


int main() {

    int page_size = init_page_size(); //tells us the size of a page on our system;

    malloc_setfsm(FIRST_FIT);
    void *memory1 = mem_alloc(104);
    void *memory2 = mem_alloc(1048);
    void *memory3 = mem_alloc(552);

    my_free(memory1);

    malloc_print();
    my_free(memory2);
    my_free(memory3);

    // malloc_setfsm(WORST_FIT);
    // void *memory4 = mem_alloc(256);

    malloc_print();
    return 0;

}