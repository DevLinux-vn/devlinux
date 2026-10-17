/**
 * @file    main.c
 * @brief   Demo program: memset, memcpy vs strncpy, memcmp vs strcmp,
 *          and memmove on overlapping regions.
 */
#include <stdio.h>
#include <string.h>
#include "memory_ops.h"

/**
 * @brief   Format @p len bytes of @p p_data as space-separated hex
 *          pairs (e.g. "01 00 FF") into @p p_out.
 *
 * @param[out] p_out    Destination text buffer.
 * @param[in]  out_size Size of @p p_out, in bytes.
 * @param[in]  p_data   Bytes to format.
 * @param[in]  len      Number of bytes in @p p_data.
 */
static void format_hex_bytes(char *p_out, size_t out_size,
                              const uint8_t *p_data, size_t len)
{
    size_t pos = 0U;
    size_t i;

    for (i = 0U; (i < len) && (pos < out_size); i++)
    {
        int written = (i == 0U)
            ? snprintf(p_out + pos, out_size - pos, "%02X", p_data[i])
            : snprintf(p_out + pos, out_size - pos, " %02X", p_data[i]);

        if (written > 0)
        {
            pos += (size_t)written;
        }
    }
}

/**
 * @brief   Format each byte of @p p_data (assumed 0-9) as a single
 *          decimal digit character, for a compact readable label.
 *
 * @param[out] p_out    Destination text buffer.
 * @param[in]  out_size Size of @p p_out, in bytes.
 * @param[in]  p_data   Bytes to format (each expected to be 0-9).
 * @param[in]  len      Number of bytes in @p p_data.
 */
static void format_digit_label(char *p_out, size_t out_size,
                                const uint8_t *p_data, size_t len)
{
    size_t i = 0U;

    while ((i < len) && (i < (out_size - 1U)))
    {
        p_out[i] = (char)('0' + p_data[i]);
        i++;
    }
    p_out[i] = '\0';
}

/**
 * @brief   Program entry point.
 *
 * @return  0 on success.
 */
int main(void)
{
    sensor_data_t sensor;
    uint8_t payload[8] = {0x01, 0x00, 0xFF, 0x00, 0x10, 0x20, 0x30, 0x40};
    uint8_t dst_strncpy[8];
    uint8_t dst_memcpy[8];
    uint8_t id1[4] = {0U, 1U, 0U, 5U};
    uint8_t id2[4] = {0U, 1U, 0U, 9U};
    char buf[16] = "123456789";
    char hex_line[32];
    char label1[8];
    char label2[8];
    int strcmp_result;
    int memcmp_result;

    printf("========== L10 Lab 4: Memory vs String Operations ==========\n\n");

    /* --- memset --- */
    printf("--- memset ---\n");
    sensor.id = 42;
    sensor.temp = 36.5f;
    sensor_data_zero_init(&sensor);
    printf("Struct initialized to zeros.\n\n");

    /* --- memcpy vs strncpy --- */
    printf("--- memcpy vs strcpy ---\n");
    format_hex_bytes(hex_line, sizeof(hex_line), payload, sizeof(payload));
    printf("Payload: %s\n", hex_line);

    copy_with_strncpy(dst_strncpy, sizeof(dst_strncpy), payload, sizeof(payload));
    format_hex_bytes(hex_line, sizeof(hex_line), dst_strncpy, sizeof(dst_strncpy));
    printf("strncpy result: %s (TRUNCATED at 0x00!)\n", hex_line);

    copy_with_memcpy(dst_memcpy, payload, sizeof(payload));
    format_hex_bytes(hex_line, sizeof(hex_line), dst_memcpy, sizeof(dst_memcpy));
    printf("memcpy result : %s (SUCCESS!)\n\n", hex_line);

    /* --- memcmp vs strcmp --- */
    printf("--- memcmp vs strcmp ---\n");
    format_digit_label(label1, sizeof(label1), id1, sizeof(id1));
    format_digit_label(label2, sizeof(label2), id2, sizeof(id2));

    strcmp_result = compare_with_strcmp(id1, id2);
    printf("strcmp(\"%s\", \"%s\") -> %d (INCORRECT: strings match due to early 0x00)\n",
           label1, label2, strcmp_result);

    memcmp_result = compare_with_memcmp(id1, id2, sizeof(id1));
    printf("memcmp(\"%s\", \"%s\") -> %d (CORRECT: arrays differ)\n\n",
           label1, label2, memcmp_result);

    /* --- memmove --- */
    printf("--- memmove ---\n");
    printf("Before shift: %s\n", buf);
    /* Shift the "456789" region (offset 3, length 6) three bytes to
     * the right to make room for new data at the front. */
    safe_shift_right(buf, sizeof(buf), 3U, 6U, 3U);
    printf("After shift : %s\n", buf);

    return 0;
}