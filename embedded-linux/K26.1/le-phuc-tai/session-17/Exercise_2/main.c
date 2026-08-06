/**
 * @file main.c
 * @brief User-space LED blinking application utilizing timerfd and state confirmation.
 * @date 2026-08-06
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>
#include <sys/timerfd.h>

#define DEVICE_PATH      "/dev/led_blink"
#define BLINK_PERIOD_SEC 5
#define TIME_BUF_SIZE    64

#define CMD_ON           '1'
#define CMD_OFF          '0'
#define SUCCESS_CODE     0
#define ERROR_CODE       1

static volatile sig_atomic_t g_keep_running = 1;

static void handle_signal(int sig)
{
	(void)sig;
	g_keep_running = 0;
}

static void get_timestamp(char *buf, size_t size)
{
	struct timespec ts;
	struct tm tm_info;

	if (clock_gettime(CLOCK_REALTIME, &ts) == 0) {
		localtime_r(&ts.tv_sec, &tm_info);
		snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d",
		         tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
		         tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec);
	} else {
		snprintf(buf, size, "UNKNOWN_TIME");
	}
}

static char read_led_status_exact(int fd)
{
	char status = '0';
	ssize_t ret;

	/* Pre-positioned read at offset 0 to query the driver state directly */
	ret = pread(fd, &status, 1, 0);
	if (ret < 0) {
		perror("[APP] ERROR: Failed to read status from device node");
		return 'E';
	}

	return status;
}

int main(void) {
	int dev_fd = -1;
	int timer_fd = -1;
	struct sigaction sa;
	struct itimerspec its;
	char current_cmd = CMD_ON;
	char time_str[TIME_BUF_SIZE];
	uint64_t expirations;
	ssize_t s_ret;

	setbuf(stdout, NULL);

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_signal;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	dev_fd = open(DEVICE_PATH, O_RDWR);
	if (dev_fd < 0) {
		fprintf(stderr, "[APP] ERROR: Cannot open device %s: %s\n", DEVICE_PATH, strerror(errno));
		return ERROR_CODE;
	}

	/* Read and output initial state right after opening the device */
	char initial_status = read_led_status_exact(dev_fd);
	get_timestamp(time_str, sizeof(time_str));
	printf("[%s] [APP] Initial LED status: %c\n", time_str, initial_status);

	/* Initialize high-precision timerfd with 5-second interval */
	timer_fd = timerfd_create(CLOCK_MONOTONIC, 0);
	if (timer_fd < 0) {
		perror("[APP] ERROR: Failed to create timerfd instance");
		close(dev_fd);
		return ERROR_CODE;
	}

	memset(&its, 0, sizeof(its));
	its.it_value.tv_sec = BLINK_PERIOD_SEC;
	its.it_interval.tv_sec = BLINK_PERIOD_SEC;

	if (timerfd_settime(timer_fd, 0, &its, NULL) < 0) {
		perror("[APP] ERROR: Failed to configure timerfd interval");
		close(timer_fd);
		close(dev_fd);
		return ERROR_CODE;
	}

	while (g_keep_running) {
		s_ret = read(timer_fd, &expirations, sizeof(expirations));
		if (s_ret < 0) {
			if (errno == EINTR) {
				continue;
			}
			perror("[APP] ERROR: timerfd read operation failed");
			break;
		}

		/* Send state toggle command */
		s_ret = write(dev_fd, &current_cmd, 1);
		if (s_ret < 0) {
			perror("[APP] ERROR: Failed to write command to LED device");
			break;
		}

		/* Hardware confirmation via immediate read-back */
		char read_back = read_led_status_exact(dev_fd);
		get_timestamp(time_str, sizeof(time_str));

		if (read_back != current_cmd) {
			printf("[%s] [APP] WARNING: status mismatch! Written='%c', ReadBack='%c'\n",
			       time_str, current_cmd, read_back);
		} else {
			printf("[%s] [APP] LED %s (status=%c)\n",
			       time_str, (current_cmd == CMD_ON) ? "ON" : "OFF", read_back);
		}

		/* Toggle target command for next 5-second iteration */
		current_cmd = (current_cmd == CMD_ON) ? CMD_OFF : CMD_ON;
	}

	printf("\n[APP] Terminating application cleanly...\n");
	if (timer_fd >= 0) close(timer_fd);
	if (dev_fd >= 0) close(dev_fd);

	return SUCCESS_CODE;
}