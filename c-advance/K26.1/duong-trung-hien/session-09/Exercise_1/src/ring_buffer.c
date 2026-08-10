#include "ring_buffer.h"

bool ring_buf_push(int val) { 
    (void)val;

    return true;
}

bool ring_buf_pop(int *val) { 
    *val = 3; 
    
    return true; 
}