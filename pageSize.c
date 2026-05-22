#include <unistd.h>
#include <stdio.h>
#include "pageSize.h"

size_t page_size = 0;

int init_page_size() {
    
    if(page_size == 0) {
        page_size = sysconf(_SC_PAGESIZE); // check the size of a page cause it could differ on different processors.
        printf("page size = %ld\n", page_size);
    }

   return page_size;

}

