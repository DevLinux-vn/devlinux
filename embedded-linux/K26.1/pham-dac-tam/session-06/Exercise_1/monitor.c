#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#define TIME_STR_MAX 64

volatile sig_atomic_t running = 1;

void sigterm_handler(int sig __attribute__((unused))) {
	printf("Service shutting down...\n");
	running = 0;
}

int main() {
	setbuf(stdout, NULL);

	/* Set up SIGTERM handler using sigaction() for POSIX compliance */
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigterm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		perror("sigaction");
		return 1;
	}

	int count = 0;
	while (running) {
		time_t now = time(NULL);
		if (now == (time_t) -1) {
			perror("time");
			return 1;
		}

		struct tm *tm_info = localtime(&now);
		if (tm_info == NULL) {
			perror("localtime");
			return 1;
		}

		char time_str[TIME_STR_MAX];
		if (strftime(time_str, TIME_STR_MAX, "%Y-%m-%d %H:%M:%S", tm_info) == 0) {
			fprintf(stderr, "strftime() failed\n");
			return 1;
		}

		printf("[%s] Monitor running - cycle %d\n", time_str, count++);
		sleep(1);
	}

	return 0;
}
