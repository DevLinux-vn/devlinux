#include "device_file_io.h"
#include <stdio.h>

int main(void)
{
    const char *path = "registers.bin";
    uint32_t regs_out[4] = {0x1A3C0B05UL, 0x5678FF02UL, 0x00001201UL, 0xABCD6409UL};
    uint32_t regs_in[4] = {0};
    reg_fields_t parsed;

    printf("=== Device File Reader with Bitwise Parsing ===\n\n");
    
    printf("--- Step 1: Creating binary register snapshot file ---\n");
    printf("Writing 4 simulated registers to registers.bin...\n");
    for (uint32_t i = 0; i < 4U; i++)
    {
        printf("  REG[%u] = 0x%08X (written)\n", i, regs_out[i]);
    }
    
    if (reg_snapshot_write(path, regs_out, 4U) == 0)
    {
        printf("Write complete (16 bytes).\n\n");
    }

    printf("--- Step 2: Reading register snapshots ---\n");
    printf("Reading registers.bin with open()/read()...\n");
    int32_t count = reg_snapshot_read(path, regs_in, 4U);
    if (count > 0)
    {
        printf("Read %d bytes (%d registers).\n\n", count * 4, count);
    }

    printf("--- Step 3: Parsing register fields ---\n");
    for (int32_t i = 0; i < count; i++)
    {
        reg_snapshot_parse(regs_in[i], &parsed);
        printf("Register[%d] = 0x%08X:\n", i, regs_in[i]);
        printf("  Bit[0]    Enabled:   %s\n", parsed.enabled ? "YES" : "NO");
        printf("  Bits[3:1] Mode:      %u\n", parsed.mode);
        printf("  Bits[15:8] Threshold: %u\n", parsed.threshold);
        printf("  Bits[31:16] Sensor:    %u\n\n", parsed.sensor_raw);
    }

    printf("=== All register snapshots parsed successfully ===\n");

    return 0;
}