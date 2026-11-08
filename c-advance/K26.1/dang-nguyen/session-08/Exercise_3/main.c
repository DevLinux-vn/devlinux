#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_PAYLOAD_SIZE 64
#define START_BYTE       0xAA

typedef enum e_packet_index {
    E_PACKET_START_BYTE = 0U,
    E_PACKET_LEN,
    E_PACKET_PAYLOAD
} e_packet_index_t;

typedef struct st_packet {
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} st_packet_t;

void parse_packet(const uint8_t *p_raw_data)
{
    st_packet_t pkt;
    
#if defined(VULNERABLE)
    if (START_BYTE == p_raw_data[E_PACKET_START_BYTE])
#else
    if ((START_BYTE == p_raw_data[E_PACKET_START_BYTE]) && (p_raw_data[E_PACKET_LEN] <= MAX_PAYLOAD_SIZE))
#endif
    {
        pkt.length = p_raw_data[E_PACKET_LEN];

        /**
         * When VULNERABLE, ⚠️ DANGER: Blindly trusting sender's length! Buffer overflow possible.
         * Otherwise, it's OK.
         */
        memcpy(pkt.payload, &p_raw_data[E_PACKET_PAYLOAD], pkt.length);

        printf("Successfully parsed packet of length: %d\n", pkt.length);
    }    
}

int main(void)
{
    /* Simulated malicious packet: Claims length is 200, but buffer is only 64! */
    uint8_t malicious_uart_data[] = {0xAA, 200, 0x01, 0x02, 0x03}; 
    parse_packet(malicious_uart_data);
    return 0;
}