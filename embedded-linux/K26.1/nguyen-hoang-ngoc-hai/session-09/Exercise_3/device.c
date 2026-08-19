#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"
#define DEVICE_POLL_INTERVAL 1

typedef struct {
	pthread_mutex_t mutex;
	int status;		/* 0 = OFF, 1 = ON */
} device_state_t;

static device_state_t *g_state = NULL;

static void cleanup(int sig)
{
	(void)sig;

	printf("\n[Device] Detaching shared memory. Goodbye.\n");

	if (g_state != NULL) {
		if (munmap(g_state, sizeof(device_state_t)) == -1)
			perror("munmap");

		g_state = NULL;
	}

	exit(0);
}

static void die(const char *msg)
{
	perror(msg);

	if (g_state != NULL)
		munmap(g_state, sizeof(device_state_t));

	exit(1);
}

int main(void)
{
	int fd;

	if (signal(SIGINT, cleanup) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	fd = shm_open(SHM_NAME, O_RDWR, 0666);
	if (fd == -1) {
		if (errno == ENOENT)
			fprintf(stderr, "[Device] Shared memory not found. Run controller first.\n");
		else
			perror("shm_open");

		exit(1);
	}

	g_state = mmap(NULL, sizeof(device_state_t),
		       PROT_READ | PROT_WRITE,
		       MAP_SHARED, fd, 0);

	if (g_state == MAP_FAILED) {
		g_state = NULL;
		close(fd);
		perror("mmap");
		exit(1);
	}

	if (close(fd) == -1)
		die("close");

	printf("[Device] Attached to %s\n", SHM_NAME);

	while (1) {
		int current_status;

		if (pthread_mutex_lock(&g_state->mutex) != 0)
			die("pthread_mutex_lock");

		current_status = g_state->status;

		if (pthread_mutex_unlock(&g_state->mutex) != 0)
			die("pthread_mutex_unlock");

		if (current_status == 1)
			printf("[Device] Status: ON  — Running...\n");
		else
			printf("[Device] Status: OFF — Idle.\n");

		sleep(DEVICE_POLL_INTERVAL);
	}

	return 0;
}