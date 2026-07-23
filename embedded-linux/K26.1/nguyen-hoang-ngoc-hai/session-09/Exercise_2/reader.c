#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "device_cfg.h"

#define READER_POLL_INTERVAL 2

static device_cfg_t *g_cfg = NULL;

static const char *log_level_name(int level)
{
	switch (level) {
	case 0:
		return "OFF";
	case 1:
		return "ERROR";
	case 2:
		return "INFO";
	case 3:
		return "DEBUG";
	default:
		return "UNKNOWN";
	}
}

static void cleanup(int sig)
{
	(void)sig;

	printf("\n[Config Reader] Cleaning up. Goodbye.\n");

	if (g_cfg != NULL) {
		if (munmap(g_cfg, sizeof(device_cfg_t)) == -1)
			perror("munmap");
	}

	exit(0);
}

int main(void)
{
	int fd;
	device_cfg_t *cfg;

	if (signal(SIGINT, cleanup) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	fd = open(CFG_PATH, O_RDONLY);
	if (fd == -1) {
		if (errno == ENOENT)
			fprintf(stderr, "Config file not found. Run config-writer first.\n");
		else
			perror("open");

		exit(1);
	}

	cfg = mmap(NULL, sizeof(device_cfg_t),
		   PROT_READ,
		   MAP_SHARED, fd, 0);

	if (cfg == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(1);
	}

	g_cfg = cfg;

	if (close(fd) == -1) {
		perror("close");
		munmap(cfg, sizeof(device_cfg_t));
		exit(1);
	}

	printf("[Config Reader] Polling %s every 2s...\n", CFG_PATH);

	while (1) {
		printf("baud_rate=%d  sampling_rate=%d Hz  log_level=%s\n",
		       cfg->baud_rate,
		       cfg->sampling_rate_hz,
		       log_level_name(cfg->log_level));

		sleep(READER_POLL_INTERVAL);
	}

	return 0;
}