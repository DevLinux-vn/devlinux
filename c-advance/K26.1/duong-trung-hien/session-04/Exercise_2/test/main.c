/**
    size build/output:
    text    data     bss     dec     hex filename
    2211     604     384    3199     c7f build/output

1. Look at the bss value. The pool array s_pool[5] and s_in_use[5] are both static variables with no 
explicit initialiser. Which memory section do they live in (.text, .data, or .bss) and why?
    - The variables s_pool[5] and s_in_use[5] are stored in the .bss memory section.
    - They are declared as static variables without an explicit initializer, so the compiler 
    places them in the .bss section. The .bss section is used for global or static variables that 
    are either uninitialized or initialized to zero.

2. If you changed the pool declaration to initialise it explicitly: static network_packet_t s_pool[5] = {0}; 
Would it move from .bss to .data? Why or why not? 
    - No, changing the declaration to: static network_packet_t s_pool[5] = {0};
    does not necessarily move it from .bss to .data.
    - Although the variable now has an explicit initializer, the compiler usually recognizes 
    that the value is zero-initialized and still places it in the .bss section to save space 
    in the executable file.
    - The .data section is mainly used for global/static variables with non-zero initial values, 
    because their initial values must be stored inside the binary file and copied to RAM during startup.

3. The packet_alloc() and packet_free() function bodies (compiled machine code) live in which memory section?
On a real MCU with 64KB Flash and 16KB RAM, where would each section physically reside?
    - The function bodies of packet_alloc() and packet_free() are compiled into machine code, so they are 
    stored in the .text section.
    - The .text section contains the executable instructions of the program, including all function code.
    - On a real MCU with:
        - 64KB Flash
        - 16KB RAM
        - The memory layout would be:
        Flash (64KB):
        +----------------+
        | .text          |  <-- packet_alloc(), packet_free(), main(), other functions
        |                |
        +----------------+
        | .data initial  |  <-- initial values of global/static variables
        +----------------+

        RAM (16KB):
        +----------------+
        | .data          |  <-- global/static variables with non-zero initial values
        +----------------+
        | .bss           |  <-- s_pool[5], s_in_use[5], zero-initialized variables
        +----------------+
        | stack          |
        +----------------+
        | heap (if used) |
        +----------------+

4. If POOL_SIZE was increased from 5 to 50, which section grows and by how much? Calculate 
the expected new .bss size in bytes, given sizeof(network_packet_t) = 68 bytes.
    - The pool array is declared as: static network_packet_t s_pool[POOL_SIZE];
    - When POOL_SIZE increases from 5 to 50, the memory used by s_pool increases.
    - Because s_pool is a static variable without an explicit non-zero initializer, it is stored in the .bss section.
    - The increase in .bss size is: New size - Old size = (50 - 5) * sizeof(network_packet_t) = 45 * 68 = 3060 bytes
    - Therefore, the .bss section grows by 3060 bytes.
    - If the original .bss size was 384 bytes: New .bss size = 384 + 3060 = 3444 bytes
 */

#include "object_pool_manager.h"
#include <stdio.h>

static uint32_t s_alloc_attempt = 1U;

static network_packet_t* allocate_print(void)
{
    network_packet_t* p_packet = packet_alloc();
    if (p_packet == NULL)
    {
        printf("Allocating packet %u: Failed (Pool Full)\n", s_alloc_attempt);
    }
    else
    {
        printf("Allocating packet %u: Success\n", s_alloc_attempt);
        s_alloc_attempt++;
    }

    return p_packet;
}

static void free_print(network_packet_t *p_packet)
{
    packet_free(p_packet);
    printf("Freeing packet %u...\n", (p_packet->id)+1);
}

int main()
{
    pool_init();
    network_packet_t* p_1 = allocate_print();
    network_packet_t* p_2 = allocate_print();
    network_packet_t* p_3 = allocate_print();
    network_packet_t* p_4 = allocate_print();
    network_packet_t* p_5 = allocate_print();
    network_packet_t* p_6 = allocate_print();
    free_print(p_2);
    network_packet_t* p_6_ = allocate_print();

    (void)p_1;
    (void)p_3;
    (void)p_4;
    (void)p_5;
    (void)p_6;
    (void)p_6_;

    return 0;
}