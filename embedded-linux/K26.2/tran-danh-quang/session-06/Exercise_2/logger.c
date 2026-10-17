#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define LOG_ERR     "<3>"
#define LOG_WARNING "<4>"
#define LOG_INFO    "<6>"

int main(void) {
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "setvbuf stdout failed: %s\n", strerror(errno));
        return 1;
    }
    if (setvbuf(stderr, NULL, _IONBF, 0) != 0) {
        fprintf(stderr, "setvbuf stderr failed: %s\n", strerror(errno));
        return 1;
    }

    int cycle = 0;
    time_t start_time = time(NULL);
    if (start_time == (time_t)-1) {
        fprintf(stderr, LOG_ERR "Failed to get current time\n");
        return 1;
    }
    /* Seed PRNG from /dev/urandom for better randomness in simulated memory % values.
     * Fallback to time() if /dev/urandom unavailable. */
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        unsigned int seed;
        ssize_t n;
        while ((n = read(fd, &seed, sizeof(seed))) == -1 && errno == EINTR);
        if (n == sizeof(seed)) {
            srand(seed);
        }
        if (close(fd) == -1) {
            perror("close");
        }
    } else {
        srand((unsigned int)start_time);
    }

    while (1) {
        cycle++;
        /* Check fprintf() return to catch I/O errors; all three log lines within same cycle should be checked consistently */
        if (fprintf(stderr, LOG_INFO "Service running normally, cycle %d\n", cycle) < 0) {
            perror("fprintf");
            exit(1);
        }
        if (fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", 80 + rand() % 15) < 0) {
            perror("fprintf");
            exit(1);
        }
        if (fprintf(stderr, LOG_ERR "Failed to connect to database, retry %d\n", cycle) < 0) {
            perror("fprintf");
            exit(1);
        }

        sleep(2);

        // Simulate crash after 30s to test systemd restart behavior
        if (time(NULL) - start_time >= 30) {
            fprintf(stderr, LOG_ERR "Simulating a crash after 30 seconds...\n");
            abort();
        }
    }

    return 0;
}
