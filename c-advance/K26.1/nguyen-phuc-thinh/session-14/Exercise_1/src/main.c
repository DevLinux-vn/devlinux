#include "posix_logger.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int ret = 0;
    FILE *p_file = NULL;
    int32_t fd = -1;

    printf("=== Part A: Standard I/O (stdio) Log Writer ===\n");
    p_file = fopen("stdio_sensor.log", "a");
    if (p_file == NULL)
    {
        perror("fopen failed");
        ret = EXIT_FAILURE;
        goto cleanup;
    }
    printf("[stdio] Opened stdio_sensor.log with fopen()\n");
    
    fprintf(p_file, "[%05d] SENSOR: temp=23.5C humidity=55%%\n", 1);
    printf("[stdio] Entry [00001] written: temp=23.5C, humidity=55%%\n");
    fprintf(p_file, "[%05d] SENSOR: temp=24.1C humidity=53%%\n", 2);
    printf("[stdio] Entry [00002] written: temp=24.1C, humidity=53%%\n");
    fprintf(p_file, "[%05d] SENSOR: temp=23.8C humidity=56%%\n", 3);
    printf("[stdio] Entry [00003] written: temp=23.8C, humidity=56%%\n");
    
    if (fclose(p_file) != 0)
    {
        perror("fclose failed");
    }
    p_file = NULL;
    printf("[stdio] Closed with fclose(). Buffered I/O complete.\n\n");

    printf("=== Part B: POSIX System Call Log Writer ===\n");
    fd = posix_log_open("posix_sensor.log");
    if (fd < 0)
    {
        perror("posix_log_open failed");
        ret = EXIT_FAILURE;
        goto cleanup;
    }
    printf("[posix] Opened posix_sensor.log with open() -> fd=%d\n", fd);

    if (posix_log_write(fd, 1U, 23.5f, 55U) == 0)
        printf("[posix] Entry [00001] written (42 bytes via write_all): temp=23.5C, humidity=55%%\n");
    if (posix_log_write(fd, 2U, 24.1f, 53U) == 0)
        printf("[posix] Entry [00002] written (42 bytes via write_all): temp=24.1C, humidity=53%%\n");
    if (posix_log_write(fd, 3U, 23.8f, 56U) == 0)
        printf("[posix] Entry [00003] written (42 bytes via write_all): temp=23.8C, humidity=56%%\n");

    if (posix_log_close(fd) < 0)
    {
        perror("posix_log_close failed");
    }
    fd = -1;
    printf("[posix] Closed with close(). Raw system call I/O complete.\n\n");

    printf("=== Part C: fcntl() Non-Blocking Demo ===\n");
    fd = posix_log_open("posix_sensor.log");
    if (fd >= 0)
    {
        if (posix_set_nonblocking(fd) == 0)
        {
            printf("[fcntl] Set O_NONBLOCK on fd using fcntl(F_GETFL/F_SETFL)\n");
            printf("[fcntl] Non-blocking mode enabled successfully.\n\n");
        }
        posix_log_close(fd);
        fd = -1;
    }

    printf("=== Comparison Summary ===\n");
    printf("stdio:  FILE * streams | Buffered by libc | Portable (ISO C)\n");
    printf("POSIX:  int fd         | Unbuffered (raw) | Linux/macOS only (POSIX)\n");
    printf("Note:   fopen() internally calls open(). They are layers, not alternatives.\n");

cleanup:
    if (p_file != NULL)
    {
        fclose(p_file);
    }
    if (fd >= 0)
    {
        posix_log_close(fd);
    }
    return ret;
}