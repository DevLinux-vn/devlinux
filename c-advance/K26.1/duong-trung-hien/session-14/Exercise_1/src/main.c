#include "posix_logger.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define APP_SUCCESS (0)

int32_t main() {
  /*  Part A: Standard I/O */
  FILE *file;

  file = fopen("stdio_sensor.log", "w");
  if (NULL == file) {
    perror("fopen");
    return 1;
  }

  printf("=== Part A: Standard I/O (stdio) Log Writer ===\n");
  printf("[stdio] Opened stdio_sensor.log with fopen()\n");

  fprintf(file, "Entry [00001] written: temp=23.5C, humidity=55%%\n");
  printf("[stdio] Entry [00001] written: temp=23.5C, humidity=55%%\n");
  fprintf(file, "Entry [00002] written: temp=24.1C, humidity=53%%\n");
  printf("[stdio] Entry [00002] written: temp=24.1C, humidity=53%%\n");
  fprintf(file, "Entry [00003] written: temp=23.8C, humidity=56%%\n");
  printf("[stdio] Entry [00003] written: temp=23.8C, humidity=56%%\n");

  if (fclose(file) != 0) {
    perror("fclose");
    return 1;
  }

  printf("[stdio] Closed with fclose(). Buffered I/O complete.\n");

  /* Part B — POSIX */
  int32_t fd;
  int32_t bytes_written;
  const char *entry;

  fprintf(stdout, "\n=== Part B: POSIX System Call Log Writer ===\n");

  fd = posix_log_open("posix_sensor.log");
  if (fd < 0) {
    return 1;
  }

  fprintf(stdout, "[posix] Opened posix_sensor.log with open() -> fd=%d\n", fd);

  entry = "Entry [00001]: temp=23.5C, humidity=55%%\n";
  bytes_written = posix_write_all(fd, (const uint8_t *)entry, strlen(entry));
  if (bytes_written < 0) {
    posix_log_close(fd);
    return 1;
  }

  fprintf(stdout,
          "[posix] Entry [00001] written (%d bytes via write_all): temp=23.5C, "
          "humidity=55%%\n",
          bytes_written);

  entry = "Entry [00002]: temp=24.1C, humidity=53%%\n";
  bytes_written = posix_write_all(fd, (const uint8_t *)entry, strlen(entry));
  if (bytes_written < 0) {
    posix_log_close(fd);
    return 1;
  }

  fprintf(stdout,
          "[posix] Entry [00002] written (%d bytes via write_all): temp=24.1C, "
          "humidity=53%%\n",
          bytes_written);

  entry = "Entry [00003]: temp=23.8C, humidity=56%%\n";
  bytes_written = posix_write_all(fd, (const uint8_t *)entry, strlen(entry));
  if (bytes_written < 0) {
    posix_log_close(fd);
    return 1;
  }

  fprintf(stdout,
          "[posix] Entry [00003] written (%d bytes via write_all): temp=23.8C, "
          "humidity=56%%\n",
          bytes_written);

  if (posix_log_close(fd) < 0) {
    return 1;
  }

  fprintf(stdout,
          "[posix] Closed with close(). Raw system call I/O complete.\n");

  /* Part C — fcntl() */
  fprintf(stdout, "\n=== Part C: fcntl() Non-Blocking Demo ===\n");

  fd = posix_log_open("fcntl_demo.log");
  if (fd < 0) {
    return 1;
  }

  if (posix_set_nonblocking(fd) < 0) {
    posix_log_close(fd);
    return 1;
  }

  fprintf(stdout,
          "[fcntl] Set O_NONBLOCK on fd using fcntl(F_GETFL/F_SETFL)\n");

  fprintf(stdout, "[fcntl] Non-blocking mode enabled successfully.\n");

  if (posix_log_close(fd) < 0) {
    return 1;
  }

  fprintf(stdout, "\n=== Comparison Summary ===\n");
  fprintf(stdout,
          "stdio:  FILE * streams | Buffered by libc    | Portable (ISO C)\n");
  fprintf(stdout, "POSIX:  int fd         | Unbuffered (raw)    | Linux/macOS "
                  "only (POSIX)\n");
  fprintf(stdout, "Note:   fopen() internally calls open(). They are layers, "
                  "not alternatives.\n");

  return APP_SUCCESS;
}
