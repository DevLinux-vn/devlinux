#include <stdint.h>   
#include <stdbool.h>  
#include <stdio.h>    
#include <stddef.h>   

#define POOL_SIZE 5

typedef struct {
    uint32_t id;
    uint8_t payload[64];
} network_packet_t;

static network_packet_t s_pool[POOL_SIZE];
static bool packet_in_use[POOL_SIZE];

/**
 * @brief Initialize the object pool (mark all as free)
 */
void pool_init(void)
{
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        packet_in_use[i] = false;
    }
}

/**
 * @brief Allocate a packet from the pool.
 * @return Pointer to packet, or NULL if pool is full.
 */
network_packet_t* packet_alloc(void)
{
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        if (false == packet_in_use[i]) {
            packet_in_use[i] = true;
            return &s_pool[i];
        }
    }

    return NULL;
}

/**
 * @brief Free a packet back to the pool.
 * @param p_packet Pointer to the packet to free.
 */
void packet_free(network_packet_t *p_packet)
{
    if (NULL == p_packet) {
        return;
    }

    ptrdiff_t index = p_packet - s_pool;

    if ((index >= 0) && (index < POOL_SIZE)) {
        packet_in_use[index] = false;
    }
}
int main(void)
{
    pool_init();  /* Khởi tạo pool: đánh dấu tất cả 5 ô là "trống" */

    network_packet_t *p_arr[6];  /* Mảng tạm lưu 6 con trỏ để test (index 0-5 tương ứng packet 1-6) */

    /* --- Cấp phát 5 packet đầu tiên --- */
    for (uint32_t i = 0U; i < POOL_SIZE; i++) {
        p_arr[i] = packet_alloc();
        if (p_arr[i] != NULL) {
            printf("Allocating packet %u: Success\n", i + 1U);
        } else {
            printf("Allocating packet %u: Failed\n", i + 1U);
        }
    }

    /* --- Thử cấp phát packet thứ 6 (phải thất bại vì pool đã đầy) --- */
    p_arr[5] = packet_alloc();
    if (p_arr[5] == NULL) {
        printf("Allocating packet 6: Failed (Pool Full)\n");
    } else {
        printf("Allocating packet 6: Success\n");
    }

    /* --- Giải phóng packet số 2 (tức p_arr[1], vì index bắt đầu từ 0) --- */
    printf("Freeing packet 2...\n");
    packet_free(p_arr[1]);
    p_arr[1] = NULL;  /* Đặt lại NULL sau khi free, tránh dùng nhầm con trỏ đã free (dangling pointer) */

    /* --- Cấp phát lại lần nữa, lần này phải thành công vì vừa có 1 ô trống --- */
    p_arr[5] = packet_alloc();
    if (p_arr[5] != NULL) {
        printf("Allocating packet 6 again: Success\n");
    } else {
        printf("Allocating packet 6 again: Failed\n");
    }

    return 0;
}