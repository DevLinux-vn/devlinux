#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "device_cfg.h"

#define POLL_INTERVAL_SEC  2

static volatile sig_atomic_t g_running = 1;

static void signal_handler(int signal_number)
{
	(void)signal_number;

	g_running = 0;
}

static int register_signal_handler(void)
{
	struct sigaction action;

	action.sa_handler = signal_handler;
	action.sa_flags = 0;

	if (sigemptyset(&action.sa_mask) == -1)
	{
		perror("sigemptyset");
		return -1;
	}

	if (sigaction(SIGINT, &action, NULL) == -1)
	{
		perror("sigaction");
		return -1;
	}

	return 0;
}

static const char *log_level_to_string(int log_level)
{
	switch (log_level)
	{
	case LOG_LEVEL_OFF:
		return "OFF";

	case LOG_LEVEL_ERROR:
		return "ERROR";

	case LOG_LEVEL_INFO:
		return "INFO";

	case LOG_LEVEL_DEBUG:
		return "DEBUG";

	default:
		return "UNKNOWN";
	}
}

static const device_cfg_t *map_config_file(void)
{
	int file_descriptor;
	struct stat file_status;
	const device_cfg_t *config;

	file_descriptor = open(CONFIG_FILE_PATH, O_RDONLY);

	if (file_descriptor == -1)
	{
		perror("open");
		fprintf(stderr,
			"Configuration file does not exist. "
			"Run config-writer first.\n");
		return NULL;
	}

	if (fstat(file_descriptor, &file_status) == -1)
	{
		perror("fstat");

		if (close(file_descriptor) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	if (file_status.st_size < (off_t)sizeof(device_cfg_t))
	{
		fprintf(stderr,
			"Configuration file has an invalid size.\n");

		if (close(file_descriptor) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	config = mmap(NULL,
		      sizeof(device_cfg_t),
		      PROT_READ,
		      MAP_SHARED,
		      file_descriptor,
		      0);

	if (config == MAP_FAILED)
	{
		perror("mmap");

		if (close(file_descriptor) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	if (close(file_descriptor) == -1)
	{
		perror("close");

		if (munmap((void *)config, sizeof(device_cfg_t)) == -1)
		{
			perror("munmap");
		}

		return NULL;
	}

	return config;
}

static void print_config(const device_cfg_t *config)
{
	device_cfg_t snapshot;

	/*
	 * Copy the mapped values first so a single report uses one local
	 * snapshot instead of repeatedly accessing the shared mapping.
	 */
	snapshot = *config;

	printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
	       snapshot.baud_rate,
	       snapshot.sampling_rate_hz,
	       log_level_to_string(snapshot.log_level));

	fflush(stdout);
}

static void sleep_until_next_poll(void)
{
	struct timespec requested_time;
	struct timespec remaining_time;

	requested_time.tv_sec = POLL_INTERVAL_SEC;
	requested_time.tv_nsec = 0;

	while ((nanosleep(&requested_time, &remaining_time) == -1) &&
	       (errno == EINTR) &&
	       (g_running != 0))
	{
		requested_time = remaining_time;
	}
}

int main(void)
{
	const device_cfg_t *config;
	int exit_status = EXIT_SUCCESS;

	if (register_signal_handler() == -1)
	{
		return EXIT_FAILURE;
	}

	config = map_config_file();
	if (config == NULL)
	{
		return EXIT_FAILURE;
	}

	printf("[Config Reader] Polling %s every %d seconds...\n",
	       CONFIG_FILE_PATH,
	       POLL_INTERVAL_SEC);

	while (g_running != 0)
	{
		print_config(config);
		sleep_until_next_poll();
	}

	printf("\n[Config Reader] Stopping.\n");

	if (munmap((void *)config, sizeof(device_cfg_t)) == -1)
	{
		perror("munmap");
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}