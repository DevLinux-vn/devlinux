#include "telemetry.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  telemetry_packet_t packet;
  memset(&packet, 0, sizeof(packet));

  if (argc > 1 && strcmp(argv[1], "--crash") == 0) {
    printf("=== Simulating Malformed Packet Crash ===\n");
    if (fflush(stdout) == EOF) {
      return 1;
    }

    /* Pass NULL to deliberately trigger SIGSEGV and create core dump */
    telemetry_parse_packet(NULL, 0U, &packet);
  } else {
    printf("=== Normal Telemetry Processing ===\n");

    uint8_t normal_raw[6] = {0x01U, 0x02U, 0xAAU, 0xBBU, 0x00U, 0x00U};

    telemetry_status_t status =
        telemetry_parse_packet(normal_raw, sizeof(normal_raw), &packet);

    printf("Status: %d, Channel: %u, Length: %u\n", (int)status,
           packet.channel_id, packet.payload_len);
  }

  return 0;
}