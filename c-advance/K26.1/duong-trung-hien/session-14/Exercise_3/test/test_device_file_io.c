#include "device_file_io.h"
#include "unity.h"
#include <unistd.h>
#include <stdint.h>

static const char *TEST_BIN_PATH = "/tmp/test_registers.bin";

void setUp(void) { unlink(TEST_BIN_PATH); }

void tearDown(void) { unlink(TEST_BIN_PATH); }

void test_field_get_enabled(void) {
  uint32_t reg = 0x00000005UL; /* Bit 0 = 1 */
  reg_fields_t fields;
  reg_snapshot_parse(reg, &fields);
  TEST_ASSERT_EQUAL_UINT32(1U, fields.enabled);
}

void test_field_get_mode(void) {
  /* Mode = 5 → bits [3:1] = 101 → byte 0 = 0b00001010 = 0x0A */
  uint32_t reg = FIELD_PREP(REG_MODE, 5U);
  reg_fields_t fields;
  reg_snapshot_parse(reg, &fields);
  TEST_ASSERT_EQUAL_UINT32(5U, fields.mode);
}

void test_field_get_threshold(void) {
  uint32_t reg = FIELD_PREP(REG_THRESHOLD, 200U);
  reg_fields_t fields;
  reg_snapshot_parse(reg, &fields);
  TEST_ASSERT_EQUAL_UINT32(200U, fields.threshold);
}

void test_field_get_sensor(void) {
  uint32_t reg = FIELD_PREP(REG_SENSOR_RAW, 43981U); /* 0xABCD */
  reg_fields_t fields;
  reg_snapshot_parse(reg, &fields);
  TEST_ASSERT_EQUAL_UINT32(43981U, fields.sensor_raw);
}

void test_write_read_roundtrip(void) {
  uint32_t regs_out[4] = {0x1A3C0B05UL, 0x5678FF02UL, 0x00001201UL,
                          0xABCD6409UL};
  int32_t ret = reg_snapshot_write(TEST_BIN_PATH, regs_out, 4U);
  TEST_ASSERT_EQUAL_INT32(0, ret);

  uint32_t regs_in[4] = {0};
  int32_t count = reg_snapshot_read(TEST_BIN_PATH, regs_in, 4U);
  TEST_ASSERT_EQUAL_INT32(4, count);

  TEST_ASSERT_EQUAL_HEX32(regs_out[0], regs_in[0]);
  TEST_ASSERT_EQUAL_HEX32(regs_out[1], regs_in[1]);
  TEST_ASSERT_EQUAL_HEX32(regs_out[2], regs_in[2]);
  TEST_ASSERT_EQUAL_HEX32(regs_out[3], regs_in[3]);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_field_get_enabled);
  RUN_TEST(test_field_get_mode);
  RUN_TEST(test_field_get_threshold);
  RUN_TEST(test_field_get_sensor);
  RUN_TEST(test_write_read_roundtrip);
  return UNITY_END();
}