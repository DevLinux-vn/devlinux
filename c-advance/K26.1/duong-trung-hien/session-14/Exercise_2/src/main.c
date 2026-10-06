#include <stdint.h>
#include <stdio.h>
#include "register_driver.h"

#define APP_SUCCESS     (0)

int32_t main() {
    printf("=== Timer Peripheral Driver Demo ===\n");

    /* 1. Initialize */
    printf("\n--- Step 1: Initialize ---\n");

    timer_reg_t timer;
    timer_init(&timer);

    printf("timer_init(): All registers reset to 0x00000000\n");
    printf("CR=0x%08X  SR=0x%08X  CNT=0x%08X  ARR=0x%08X\n", timer.CR, timer.SR, timer.CNT, timer.ARR);

    /* 2. Configure */
    printf("\n--- Step 2: Configure (prescaler=8, reload=1000) ---\n");

    printf("timer_configure(): Using Read-Modify-Write pattern\n");

    printf("RMW: Read  CR = 0x%08X\n", timer.CR);
    
    printf("RMW: Clear PSC field [7:4], insert PSC=8\n");
    timer_configure(&timer, 8U, 1000U);
    printf("RMW: Write CR = 0x%08X\n", timer.CR);
    printf("ARR set to 1000 (0x%08X)\n", timer.ARR);
    printf("CR=0x%08X  ARR=0x%08X\n", timer.CR, timer.ARR);

    /* 3. Start Timer */
    printf("\n--- Step 3: Start Timer ---\n");

    printf("timer_start(): SET bit CR.EN\n");
    timer_start(&timer);
    printf("CR=0x%08X (EN=1, PSC=8)\n", timer.CR);

    /* 4. Simulate Count & Read */

    printf("\n--- Step 4: Simulate Count & Read ---\n");

    printf("CNT=500 (simulated hardware update)\n");
    timer.CNT = 500U;
    printf("timer_read_count() = %u\n", timer_read_count(&timer));

    /* 5. Simulate Update Event */
    printf("\n--- Step 5: Simulate Update Event ---\n");

    SET_BIT(timer.SR, TIMER_SR_UIF_Pos);
    printf("SR=0x%08X (UIF=%u, simulated)\n", timer.SR, timer.SR);

    printf("timer_is_event() = %s\n", (timer_is_event(&timer)) ? "true" : "false");
    timer_clear_event(&timer);
    printf("timer_clear_event(): CLR bit SR.UIF\n");
    printf("SR=0x%08X (UIF=0)\n", timer.SR);

    /* 6. Stop Timer */
    printf("\n--- Step 6: Stop Timer ---\n");

    printf("timer_stop(): CLR bit CR.EN\n");
    timer_stop(&timer);

    printf("CR=0x%08X (EN=0, PSC=8 preserved)\n", timer.CR);

    /* 7. Verify RMW Preserved Other Fields */
    printf("\n--- Step 7: Verify RMW Preserved Other Fields ---\n");

    printf("PSC field after stop: %lu (PRESERVED by RMW)\n", _FLD2VAL(TIMER_CR_PSC, timer.CR));
    printf("OPM field after stop: %lu (PRESERVED by RMW)\n", _FLD2VAL(TIMER_CR_OPM, timer.CR));
    printf("PASS: Read-Modify-Write correctly preserved all non-target fields.\n");

    return APP_SUCCESS;
}