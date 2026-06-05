#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pageSize.h"
#include "mem_allocator.h"


int main() {


    int page_size = init_page_size(); //tells us the size of a page on our system;
    malloc_setfsm(FIRST_FIT);
    // void *memory1 = mem_alloc(2048);
    // void *memory2 = mem_alloc(1024);
    // void *memory3 = mem_alloc(1024);
    // void *memory4 = mem_alloc(1024);

    // char* p = mem_alloc(2000);
    // strcpy(p, "I love Pumps!");
    // printf("\nBefore realloc tests... : [%s]\n", p);

    // malloc_print();

    // char *q = reallocate(p, 100);
    // printf("After reallocate..! : [%s]\n", q);

    char *p = reallocate(NULL, 100);
    strcpy(p, "I love pumps!");

    printf("\n[%s]\n", p);

    reallocate(p, 0);

    malloc_print();

    // malloc_setfsm(WORST_FIT);
    // void *memory4 = mem_alloc(256);
    return 0;

}