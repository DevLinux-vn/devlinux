#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define LOG_ERR       "<3>"
#define LOG_WARNING   "<4>"
#define LOG_INFO      "<6>"
#define LOG_INTERVAL  2
#define CRASH_TIME    30
#define MEM_BASE      80
#define MEM_RAND      15

int main() {
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	time_t start_time = time(NULL);
	if (start_time == (time_t) -1) {
		perror("time");
		return 1;
	}

	int cycle = 0;

	while (1) {
		time_t now = time(NULL);
		if (now == (time_t) -1) {
			perror("time");
			return 1;
		}

		int elapsed = (int)(now - start_time);

		/* Every LOG_INTERVAL seconds, write 3 log lines */
		if (elapsed % LOG_INTERVAL == 0 && cycle * LOG_INTERVAL <= elapsed) {
			cycle++;

			fprintf(stderr, LOG_INFO    "Service running normally, cycle %d\n", cycle);
			fprintf(stderr, LOG_WARNING "Memory usage high: %d%%\n", MEM_BASE + rand() % MEM_RAND);
			fprintf(stderr, LOG_ERR     "Failed to connect to database, retry %d\n", cycle);
		}

		/* Crash after CRASH_TIME seconds */
		if (elapsed >= CRASH_TIME) {
			fprintf(stderr, LOG_ERR "Critical error detected, aborting service\n");
			abort();
		}

		sleep(1);
	}

	return 0;
}
