#include <unistd.h>
#include <stdio.h>
#include "pageSize.h"

size_t page_size = 0;

size_t init_page_size() {
    
    if(page_size == 0) {
        page_size = sysconf(_SC_PAGESIZE); // check the size of a page cause it could differ on different processors.
    }

   return page_size;

}

