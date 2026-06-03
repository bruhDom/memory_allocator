#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pageSize.h"
#include "mem_allocator.h"


int main() {

    int page_size = init_page_size(); //tells us the size of a page on our system;

    void *memory1 = mem_alloc(4040);
    malloc_print();

    return 0;

}