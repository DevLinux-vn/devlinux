#include "memory_map.h"

int main()
{
    print_memory_map();

    return 0;
}

/**
   ---------------SIZE_OUTPUT------------------
   text    data     bss     dec     hex filename
   2820     636      12    3468     d8c output
 */

/** 
   ---------------SIZE_OUTPUT------------------
   0000000000002008 R global_const
   0000000000004010 D global_init
   0000000000004018 B global_uninit
   00000000000011dd T print_memory_map
 */