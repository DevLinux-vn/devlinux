#include "device_file_io.h"
#include <stdint.h>
#include <stdio.h>

#define APP_SUCCESS (0)
#define ARR_SIZE(arr) (sizeof(arr) / (sizeof((arr)[0])))
#define BOOL_STR(x) ((x) ? "YES" : "NO")

int32_t main() {
  printf("=== Device File Reader with Bitwise Parsing ===\n");

  const char *p_path = "register_snapshot.bin";

  const uint32_t raw_regs[] = {0x1A3C0B05UL, 0x5678FF02UL, 0x00001201UL,
                               0xABCD6409UL};

  uint32_t read_regs[MAX_SNAPSHOTS];
  reg_fields_t fields;
  uint32_t check = 0U;

  /* STEP 1 */

  printf("\n--- Step 1: Creating binary register snapshot file ---\n");
  printf("Writing 4 simulated registers to registers.bin...\n");

  for (size_t i = 0U; i < ARR_SIZE(raw_regs); i++) {
    if (reg_snapshot_write(p_path, raw_regs, 4U) < 0) {
      printf("WRITE 0x%08X FAILED\n", raw_regs[i]);
    } else {
      printf("REG[%zu] = 0x%08X (written)\n", i, raw_regs[i]);
      check++;
    }
  }

  if (check == ARR_SIZE(raw_regs)) {
    printf("Write complete (%zu bytes).\n", sizeof(raw_regs));
  }

  /* STEP 2 */

  printf("\n--- Step 2: Reading register snapshots ---\n");
  printf("Reading registers.bin with open()/read()...\n");

  int32_t ret = reg_snapshot_read(p_path, read_regs, 4U);
  printf("Read %d bytes (%d registers).\n", ret * 4, ret);

  /* STEP 3 */

  printf("\n--- Step 3: Parsing register fields ---\n");

  for (size_t i = 0U; i < ARR_SIZE(raw_regs); i++) {
    reg_snapshot_parse(read_regs[i], &fields);

    printf("Register[%zu] = 0x%08X:\n", i, read_regs[i]);
    printf("Bit[0]     Enabled:    %s\n", BOOL_STR(fields.enabled));
    printf("Bits[3:1]  Mode:       %u\n", fields.mode);
    printf("Bits[15:8] Threshold:  %u\n", fields.threshold);
    printf("Bits[31:16] Sensor:    %u\n", fields.sensor_raw);
    printf("\n");
  }

  printf("=== All register snapshots parsed successfully ===\n");

  return APP_SUCCESS;
}