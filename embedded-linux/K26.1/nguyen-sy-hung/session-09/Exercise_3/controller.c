#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME            "/device_shm"
#define SHM_PERMISSION      0666
#define INPUT_BUFFER_SIZE   64U

#define COMMAND_ON          "on"
#define COMMAND_OFF         "off"
#define COMMAND_QUIT        "quit"

#define DEVICE_STATUS_OFF   0
#define DEVICE_STATUS_ON    1

typedef struct
{
	pthread_mutex_t mutex;
	int status;
} device_state_t;

static void print_pthread_error(const char *function_name, int error_code)
{
	fprintf(stderr,
		"%s: %s\n",
		function_name,
		strerror(error_code));
}

static void remove_newline(char *buffer)
{
	size_t length;

	if (buffer == NULL)
	{
		return;
	}

	length = strlen(buffer);

	if ((length > 0U) && (buffer[length - 1U] == '\n'))
	{
		buffer[length - 1U] = '\0';
	}
}

static int read_command(char *buffer, size_t buffer_size)
{
	int character;

	if ((buffer == NULL) || (buffer_size == 0U))
	{
		return -1;
	}

	if (fgets(buffer, buffer_size, stdin) == NULL)
	{
		if (feof(stdin) != 0)
		{
			return 1;
		}

		perror("fgets");
		return -1;
	}

	if (strchr(buffer, '\n') == NULL)
	{
		do
		{
			character = getchar();
		}
		while ((character != '\n') && (character != EOF));

		fprintf(stderr, "Input is too long.\n");
		return -1;
	}

	remove_newline(buffer);

	return 0;
}

static int create_shared_memory(int *shm_fd, device_state_t **state)
{
	int local_fd;
	device_state_t *local_state;

	local_fd = shm_open(SHM_NAME,
			    O_CREAT | O_EXCL | O_RDWR,
			    SHM_PERMISSION);

	if (local_fd == -1)
	{
		if (errno == EEXIST)
		{
			fprintf(stderr,
				"Shared memory %s already exists.\n"
				"Another controller may be running, or a stale "
				"shared-memory object remains.\n",
				SHM_NAME);
		}
		else
		{
			perror("shm_open");
		}

		return -1;
	}

	if (ftruncate(local_fd, sizeof(device_state_t)) == -1)
	{
		perror("ftruncate");

		if (close(local_fd) == -1)
		{
			perror("close");
		}

		if (shm_unlink(SHM_NAME) == -1)
		{
			perror("shm_unlink");
		}

		return -1;
	}

	local_state = mmap(NULL,
			   sizeof(device_state_t),
			   PROT_READ | PROT_WRITE,
			   MAP_SHARED,
			   local_fd,
			   0);

	if (local_state == MAP_FAILED)
	{
		perror("mmap");

		if (close(local_fd) == -1)
		{
			perror("close");
		}

		if (shm_unlink(SHM_NAME) == -1)
		{
			perror("shm_unlink");
		}

		return -1;
	}

	*shm_fd = local_fd;
	*state = local_state;

	return 0;
}

static int initialize_shared_mutex(device_state_t *state)
{
	pthread_mutexattr_t attribute;
	int result;

	result = pthread_mutexattr_init(&attribute);
	if (result != 0)
	{
		print_pthread_error("pthread_mutexattr_init", result);
		return -1;
	}

	result = pthread_mutexattr_setpshared(
		&attribute,
		PTHREAD_PROCESS_SHARED);

	if (result != 0)
	{
		print_pthread_error("pthread_mutexattr_setpshared", result);

		result = pthread_mutexattr_destroy(&attribute);
		if (result != 0)
		{
			print_pthread_error(
				"pthread_mutexattr_destroy",
				result);
		}

		return -1;
	}

	result = pthread_mutex_init(&state->mutex, &attribute);
	if (result != 0)
	{
		print_pthread_error("pthread_mutex_init", result);

		result = pthread_mutexattr_destroy(&attribute);
		if (result != 0)
		{
			print_pthread_error(
				"pthread_mutexattr_destroy",
				result);
		}

		return -1;
	}

	result = pthread_mutexattr_destroy(&attribute);
	if (result != 0)
	{
		print_pthread_error("pthread_mutexattr_destroy", result);

		result = pthread_mutex_destroy(&state->mutex);
		if (result != 0)
		{
			print_pthread_error("pthread_mutex_destroy", result);
		}

		return -1;
	}

	state->status = DEVICE_STATUS_OFF;

	return 0;
}

static int update_device_status(device_state_t *state, int new_status)
{
	int result;

	result = pthread_mutex_lock(&state->mutex);
	if (result != 0)
	{
		print_pthread_error("pthread_mutex_lock", result);
		return -1;
	}

	state->status = new_status;

	result = pthread_mutex_unlock(&state->mutex);
	if (result != 0)
	{
		print_pthread_error("pthread_mutex_unlock", result);
		return -1;
	}

	return 0;
}

static int command_loop(device_state_t *state)
{
	char command[INPUT_BUFFER_SIZE];
	int input_result;

	while (1)
	{
		printf("> ");
		fflush(stdout);

		input_result = read_command(command, sizeof(command));

		if (input_result == 1)
		{
			printf("\n");
			break;
		}

		if (input_result == -1)
		{
			continue;
		}

		if (strcmp(command, COMMAND_ON) == 0)
		{
			if (update_device_status(
				    state,
				    DEVICE_STATUS_ON) == -1)
			{
				return -1;
			}

			printf("[Controller] Command sent: ON\n");
		}
		else if (strcmp(command, COMMAND_OFF) == 0)
		{
			if (update_device_status(
				    state,
				    DEVICE_STATUS_OFF) == -1)
			{
				return -1;
			}

			printf("[Controller] Command sent: OFF\n");
		}
		else if (strcmp(command, COMMAND_QUIT) == 0)
		{
			break;
		}
		else if (command[0] != '\0')
		{
			fprintf(stderr,
				"Unknown command. Use: on, off, or quit.\n");
		}
	}

	return 0;
}

static int cleanup_controller(device_state_t *state,
			      int mutex_initialized)
{
	int exit_status = 0;
	int result;

	if ((state != NULL) && (mutex_initialized != 0))
	{
		result = pthread_mutex_destroy(&state->mutex);

		if (result != 0)
		{
			print_pthread_error("pthread_mutex_destroy", result);
			exit_status = -1;
		}
	}

	if (state != NULL)
	{
		if (munmap(state, sizeof(device_state_t)) == -1)
		{
			perror("munmap");
			exit_status = -1;
		}
	}

	if (shm_unlink(SHM_NAME) == -1)
	{
		if (errno != ENOENT)
		{
			perror("shm_unlink");
			exit_status = -1;
		}
	}

	return exit_status;
}


int main(void)
{
	int shm_fd = -1;
	int mutex_initialized = 0;
	int exit_status = EXIT_SUCCESS;
	device_state_t *state = NULL;

	if (create_shared_memory(&shm_fd, &state) == -1)
	{
		return EXIT_FAILURE;
	}

	if (initialize_shared_mutex(state) == -1)
	{
		if (munmap(state, sizeof(device_state_t)) == -1)
		{
			perror("munmap");
		}

		if (close(shm_fd) == -1)
		{
			perror("close");
		}

		if (shm_unlink(SHM_NAME) == -1)
		{
			perror("shm_unlink");
		}

		return EXIT_FAILURE;
	}

	mutex_initialized = 1;

	if (close(shm_fd) == -1)
	{
		perror("close");

		cleanup_controller(state, mutex_initialized);
		return EXIT_FAILURE;
	}

	printf("[Controller] Shared memory ready. "
	       "Commands: on / off / quit\n");

	if (command_loop(state) == -1)
	{
		exit_status = EXIT_FAILURE;
	}

	printf("[Controller] Cleaning up. Goodbye.\n");

	if (cleanup_controller(state, mutex_initialized) == -1)
	{
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}