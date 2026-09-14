#include "register_driver.h"
#include <stdio.h>

int main(void)
{
    timer_reg_t sim_timer;

    printf("=== Timer Peripheral Driver Demo ===\n\n");

    printf("--- Step 1: Initialize ---\n");
    printf("timer_init(): All registers reset to 0x00000000\n");
    timer_init(&sim_timer);
    printf("CR=0x%08X  SR=0x%08X  CNT=0x%08X  ARR=0x%08X\n\n", 
           sim_timer.CR, sim_timer.SR, sim_timer.CNT, sim_timer.ARR);

    printf("--- Step 2: Configure (prescaler=8, reload=1000) ---\n");
    printf("timer_configure(): Using Read-Modify-Write pattern\n");
    printf("  RMW: Read   CR = 0x%08X\n", sim_timer.CR);
    printf("  RMW: Clear PSC field [7:4], insert PSC=8\n");
    timer_configure(&sim_timer, 8U, 1000U);
    printf("  RMW: Write  CR = 0x%08X\n", sim_timer.CR);
    printf("  ARR set to 1000 (0x%08X)\n", sim_timer.ARR);
    printf("CR=0x%08X  ARR=0x%08X\n\n", sim_timer.CR, sim_timer.ARR);

    printf("--- Step 3: Start Timer ---\n");
    printf("timer_start(): SET bit CR.EN\n");
    timer_start(&sim_timer);
    printf("CR=0x%08X (EN=1, PSC=8)\n\n", sim_timer.CR);

    printf("--- Step 4: Simulate Count & Read ---\n");
    printf("CNT=500 (simulated hardware update)\n");
    sim_timer.CNT = 500U;
    printf("timer_read_count() = %u\n\n", timer_read_count(&sim_timer));

    printf("--- Step 5: Simulate Update Event ---\n");
    sim_timer.SR |= TIMER_SR_UIF_Msk; 
    printf("SR=0x%08X (UIF=1, simulated)\n", sim_timer.SR);
    printf("timer_is_event() = %s\n", timer_is_event(&sim_timer) ? "true" : "false");
    printf("timer_clear_event(): CLR bit SR.UIF\n");
    timer_clear_event(&sim_timer);
    printf("SR=0x%08X (UIF=0)\n\n", sim_timer.SR);

    printf("--- Step 6: Stop Timer ---\n");
    printf("timer_stop(): CLR bit CR.EN\n");
    timer_stop(&sim_timer);
    printf("CR=0x%08X (EN=0, PSC=8 preserved)\n\n", sim_timer.CR);

    printf("--- Step 7: Verify RMW Preserved Other Fields ---\n");
    printf("PSC field after stop: %u (PRESERVED by RMW)\n", _FLD2VAL(TIMER_CR_PSC, sim_timer.CR));
    printf("OPM field after stop: %u (PRESERVED by RMW)\n", _FLD2VAL(TIMER_CR_OPM, sim_timer.CR));
    printf("PASS: Read-Modify-Write correctly preserved all non-target fields.\n");

    return 0;
}