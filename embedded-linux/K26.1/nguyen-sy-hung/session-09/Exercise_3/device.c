#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME              "/device_shm"
#define POLL_INTERVAL_SEC     1

#define DEVICE_STATUS_OFF     0
#define DEVICE_STATUS_ON      1

typedef struct
{
	pthread_mutex_t mutex;
	int status;
} device_state_t;

static volatile sig_atomic_t g_running = 1;

static void signal_handler(int signal_number)
{
	(void)signal_number;

	g_running = 0;
}

static void print_pthread_error(const char *function_name, int error_code)
{
	fprintf(stderr,
		"%s: %s\n",
		function_name,
		strerror(error_code));
}

static int register_signal_handler(void)
{
	struct sigaction action;

	memset(&action, 0, sizeof(action));

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

static device_state_t *attach_shared_memory(void)
{
	int shm_fd;
	struct stat file_status;
	device_state_t *state;

	shm_fd = shm_open(SHM_NAME, O_RDWR, 0);

	if (shm_fd == -1)
	{
		if (errno == ENOENT)
		{
			fprintf(stderr,
				"Controller is not running. "
				"Shared memory %s does not exist.\n",
				SHM_NAME);
		}
		else
		{
			perror("shm_open");
		}

		return NULL;
	}

	if (fstat(shm_fd, &file_status) == -1)
	{
		perror("fstat");

		if (close(shm_fd) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	if (file_status.st_size < (off_t)sizeof(device_state_t))
	{
		fprintf(stderr,
			"Shared memory has an invalid size.\n");

		if (close(shm_fd) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	state = mmap(NULL,
		     sizeof(device_state_t),
		     PROT_READ | PROT_WRITE,
		     MAP_SHARED,
		     shm_fd,
		     0);

	if (state == MAP_FAILED)
	{
		perror("mmap");

		if (close(shm_fd) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	if (close(shm_fd) == -1)
	{
		perror("close");

		if (munmap(state, sizeof(device_state_t)) == -1)
		{
			perror("munmap");
		}

		return NULL;
	}

	return state;
}

static int read_device_status(device_state_t *state, int *status)
{
	int result;

	result = pthread_mutex_lock(&state->mutex);
	if (result != 0)
	{
		print_pthread_error("pthread_mutex_lock", result);
		return -1;
	}

	*status = state->status;

	result = pthread_mutex_unlock(&state->mutex);
	if (result != 0)
	{
		print_pthread_error("pthread_mutex_unlock", result);
		return -1;
	}

	return 0;
}

static void print_device_status(int status)
{
	if (status == DEVICE_STATUS_ON)
	{
		printf("[Device] Status: ON  - Running...\n");
	}
	else if (status == DEVICE_STATUS_OFF)
	{
		printf("[Device] Status: OFF - Idle.\n");
	}
	else
	{
		printf("[Device] Status: UNKNOWN (%d)\n", status);
	}

	fflush(stdout);
}

static int sleep_until_next_poll(void)
{
	struct timespec requested_time;
	struct timespec remaining_time;

	requested_time.tv_sec = POLL_INTERVAL_SEC;
	requested_time.tv_nsec = 0;

	while (nanosleep(&requested_time, &remaining_time) == -1)
	{
		if (errno != EINTR)
		{
			perror("nanosleep");
			return -1;
		}

		if (g_running == 0)
		{
			return 0;
		}

		requested_time = remaining_time;
	}

	return 0;
}

int main(void)
{
	device_state_t *state;
	int status;
	int exit_status = EXIT_SUCCESS;

	if (register_signal_handler() == -1)
	{
		return EXIT_FAILURE;
	}

	state = attach_shared_memory();
	if (state == NULL)
	{
		return EXIT_FAILURE;
	}

	printf("[Device] Attached to %s\n", SHM_NAME);

	while (g_running != 0)
	{
		if (read_device_status(state, &status) == -1)
		{
			exit_status = EXIT_FAILURE;
			break;
		}

		print_device_status(status);

		if (sleep_until_next_poll() == -1)
		{
			exit_status = EXIT_FAILURE;
			break;
		}
	}

	printf("\n[Device] Stopping.\n");

	if (munmap(state, sizeof(device_state_t)) == -1)
	{
		perror("munmap");
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}
