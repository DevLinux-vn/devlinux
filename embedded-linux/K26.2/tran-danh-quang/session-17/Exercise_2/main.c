/*
 * LED blink client: read initial state, then hold ON/OFF for five seconds each.
 * timerfd supplies monotonic ticks; signalfd handles SIGINT/SIGTERM in the
 * main loop, so no signal handler performs I/O or closes descriptors.
 */

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/led_blink"
#define BLINK_INTERVAL_SEC 5
#define NANOSECONDS_PER_MILLISECOND 1000000L
#define POLL_FD_COUNT 2
#define SIGNAL_INDEX 0
#define TIMER_INDEX 1

static int read_status(int fd, char *status)
{
	ssize_t result;

	do {
		result = read(fd, status, sizeof(*status));
	} while (result < 0 && errno == EINTR);
	if (result < 0)
		return -1;
	if (result != sizeof(*status)) {
		errno = EIO;
		return -1;
	}
	if (*status != '0' && *status != '1') {
		errno = EPROTO;
		return -1;
	}
	return 0;
}

static int set_and_confirm(int fd, char command)
{
	struct timespec timestamp;
	ssize_t result;
	char status;

	do {
		result = write(fd, &command, sizeof(command));
	} while (result < 0 && errno == EINTR);
	if (result < 0)
		return -1;
	if (result != sizeof(command)) {
		errno = EIO;
		return -1;
	}
	/* The driver resets this open file's offset after a successful write. */
	if (read_status(fd, &status) < 0)
		return -1;
	if (clock_gettime(CLOCK_REALTIME, &timestamp) < 0)
		return -1;
	if (printf("[APP] [%jd.%03ld] LED %s (status=%c)\n",
		   (intmax_t)timestamp.tv_sec,
		   timestamp.tv_nsec / NANOSECONDS_PER_MILLISECOND,
		   command == '1' ? "ON" : "OFF", status) < 0)
		return -1;
	if (status != command &&
	    printf("[APP] WARNING: status mismatch (requested=%c, actual=%c)\n",
		   command, status) < 0)
		return -1;
	return 0;
}

static int blink_loop(int led_fd, int timer_fd, int signal_fd)
{
	struct pollfd events[POLL_FD_COUNT] = {
		{ .fd = signal_fd, .events = POLLIN },
		{ .fd = timer_fd, .events = POLLIN },
	};
	struct signalfd_siginfo signal_info;
	uint64_t expirations;
	char command = '1';
	ssize_t bytes;
	int ready;
	int i;

	if (set_and_confirm(led_fd, command) < 0)
		return -1;
	for (;;) {
		ready = poll(events, POLL_FD_COUNT, -1);
		if (ready < 0) {
			if (errno == EINTR)
				continue;
			return -1;
		}
		for (i = 0; i < POLL_FD_COUNT; i++) {
			if (events[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
				errno = EIO;
				return -1;
			}
		}
		/* Give shutdown priority if a signal and a tick arrive together. */
		if (events[SIGNAL_INDEX].revents & POLLIN) {
			bytes = read(signal_fd, &signal_info, sizeof(signal_info));
			if (bytes < 0 && errno == EINTR)
				continue;
			if (bytes != sizeof(signal_info)) {
				if (bytes >= 0)
					errno = EIO;
				return -1;
			}
			return 0;
		}
		if (!(events[TIMER_INDEX].revents & POLLIN))
			continue;
		bytes = read(timer_fd, &expirations, sizeof(expirations));
		if (bytes < 0 && errno == EINTR)
			continue;
		if (bytes != sizeof(expirations) || !expirations) {
			if (bytes >= 0)
				errno = EIO;
			return -1;
		}
		/* Skip missed transitions instead of producing a burst of toggles. */
		if (!(expirations & 1))
			continue;
		command = command == '1' ? '0' : '1';
		if (set_and_confirm(led_fd, command) < 0)
			return -1;
	}
}

int main(void)
{
	struct itimerspec interval = {
		.it_value = { .tv_sec = BLINK_INTERVAL_SEC },
		.it_interval = { .tv_sec = BLINK_INTERVAL_SEC },
	};
	struct sigaction ignore_pipe = { .sa_handler = SIG_IGN };
	sigset_t stop_signals;
	int led_fd = -1;
	int timer_fd = -1;
	int signal_fd = -1;
	int result = EXIT_FAILURE;
	char initial_status;

	if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
		perror("setvbuf stdout");
		return EXIT_FAILURE;
	}
	if (sigemptyset(&ignore_pipe.sa_mask) < 0 ||
	    sigaction(SIGPIPE, &ignore_pipe, NULL) < 0 ||
	    sigemptyset(&stop_signals) < 0 ||
	    sigaddset(&stop_signals, SIGINT) < 0 ||
	    sigaddset(&stop_signals, SIGTERM) < 0 ||
	    sigprocmask(SIG_BLOCK, &stop_signals, NULL) < 0) {
		perror("configure signals");
		goto cleanup;
	}
	signal_fd = signalfd(-1, &stop_signals, SFD_CLOEXEC);
	if (signal_fd < 0) {
		perror("signalfd");
		goto cleanup;
	}
	led_fd = open(DEVICE_PATH, O_RDWR | O_CLOEXEC);
	if (led_fd < 0) {
		perror("open " DEVICE_PATH);
		goto cleanup;
	}
	if (read_status(led_fd, &initial_status) < 0) {
		perror("read initial LED status");
		goto cleanup;
	}
	if (printf("[APP] Initial LED status: %c\n", initial_status) < 0) {
		perror("print initial LED status");
		goto cleanup;
	}
	timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC);
	if (timer_fd < 0) {
		perror("timerfd_create");
		goto cleanup;
	}
	if (timerfd_settime(timer_fd, 0, &interval, NULL) < 0) {
		perror("timerfd_settime");
		goto cleanup;
	}
	if (blink_loop(led_fd, timer_fd, signal_fd) < 0) {
		perror("LED blink loop");
		goto cleanup;
	}
	result = EXIT_SUCCESS;

cleanup:
	/* On Linux close() is never retried, even when it reports EINTR. */
	if (timer_fd >= 0 && close(timer_fd) < 0) {
		perror("close timerfd");
		result = EXIT_FAILURE;
	}
	if (led_fd >= 0 && close(led_fd) < 0) {
		perror("close LED");
		result = EXIT_FAILURE;
	}
	if (signal_fd >= 0 && close(signal_fd) < 0) {
		perror("close signalfd");
		result = EXIT_FAILURE;
	}
	if (fflush(stdout) == EOF) {
		perror("flush stdout");
		result = EXIT_FAILURE;
	}
	return result;
}
