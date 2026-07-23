#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"
#define INPUT_MAX 64

typedef struct {
	pthread_mutex_t mutex;
	int status;		/* 0 = OFF, 1 = ON */
} device_state_t;

static device_state_t *g_state = NULL;
static int g_mutex_initialized = 0;

static void cleanup(void)
{
	if (g_state != NULL) {
		if (g_mutex_initialized) {
			if (pthread_mutex_destroy(&g_state->mutex) != 0)
				fprintf(stderr, "Warning: pthread_mutex_destroy failed\n");
		}

		if (munmap(g_state, sizeof(device_state_t)) == -1)
			perror("munmap");

		g_state = NULL;
	}

	if (shm_unlink(SHM_NAME) == -1)
		perror("shm_unlink");
}

static void cleanup_signal(int sig)
{
	(void)sig;

	printf("\n[Controller] Cleaning up. Goodbye.\n");
	cleanup();
	exit(0);
}

static void die(const char *msg)
{
	perror(msg);
	cleanup();
	exit(1);
}

int main(void)
{
	int fd;
	char input[INPUT_MAX];
	pthread_mutexattr_t attr;

	if (signal(SIGINT, cleanup_signal) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (fd == -1) {
		perror("shm_open");
		exit(1);
	}

	if (ftruncate(fd, sizeof(device_state_t)) == -1) {
		close(fd);
		perror("ftruncate");
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

	if (pthread_mutexattr_init(&attr) != 0)
		die("pthread_mutexattr_init");

	if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
		pthread_mutexattr_destroy(&attr);
		die("pthread_mutexattr_setpshared");
	}

	if (pthread_mutex_init(&g_state->mutex, &attr) != 0) {
		pthread_mutexattr_destroy(&attr);
		die("pthread_mutex_init");
	}

	g_mutex_initialized = 1;

	if (pthread_mutexattr_destroy(&attr) != 0)
		die("pthread_mutexattr_destroy");

	g_state->status = 0;

	printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

	while (1) {
		printf("> ");

		if (fgets(input, sizeof(input), stdin) == NULL) {
			printf("\nEOF detected. Exiting.\n");
			break;
		}

		input[strcspn(input, "\n")] = '\0';

		if (strcmp(input, "quit") == 0)
			break;

		if (strcmp(input, "on") == 0) {
			if (pthread_mutex_lock(&g_state->mutex) != 0)
				die("pthread_mutex_lock");

			g_state->status = 1;

			if (pthread_mutex_unlock(&g_state->mutex) != 0)
				die("pthread_mutex_unlock");

			printf("[Controller] Command sent: ON\n");
		} else if (strcmp(input, "off") == 0) {
			if (pthread_mutex_lock(&g_state->mutex) != 0)
				die("pthread_mutex_lock");

			g_state->status = 0;

			if (pthread_mutex_unlock(&g_state->mutex) != 0)
				die("pthread_mutex_unlock");

			printf("[Controller] Command sent: OFF\n");
		} else {
			printf("Unknown command. Use: on / off / quit\n");
		}
	}

	printf("[Controller] Cleaning up. Goodbye.\n");

	cleanup();

	return 0;
}