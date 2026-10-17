#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <limits.h>

#include "device_cfg.h"

#define INPUT_BUFFER_SIZE  128U
#define NUMBER_BASE        10

#define COMMAND_BAUD       "baud"
#define COMMAND_RATE       "rate"
#define COMMAND_LOG        "log"
#define COMMAND_QUIT       "quit"

static const int g_supported_baud_rates[] =
{
	9600,
	115200,
	460800
};

#define SUPPORTED_BAUD_COUNT \
	(sizeof(g_supported_baud_rates) / sizeof(g_supported_baud_rates[0]))

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

static int read_input(char *buffer, size_t buffer_size)
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

static int parse_integer(const char *text, int *value)
{
	char *end_pointer;
	long parsed_value;

	if ((text == NULL) || (value == NULL) || (*text == '\0'))
	{
		return -1;
	}

	errno = 0;

	parsed_value = strtol(text, &end_pointer, NUMBER_BASE);

	if ((errno == ERANGE) ||
	    (end_pointer == text) ||
	    (*end_pointer != '\0'))
	{
		return -1;
	}

	if ((parsed_value < INT_MIN) || (parsed_value > INT_MAX))
	{
		return -1;
	}

	*value = (int)parsed_value;

	return 0;
}

static bool is_supported_baud_rate(int baud_rate)
{
	size_t index;

	for (index = 0U; index < SUPPORTED_BAUD_COUNT; index++)
	{
		if (g_supported_baud_rates[index] == baud_rate)
		{
			return true;
		}
	}

	return false;
}

static bool is_valid_config(const device_cfg_t *config)
{
	if (config == NULL)
	{
		return false;
	}

	if (!is_supported_baud_rate(config->baud_rate))
	{
		return false;
	}

	if ((config->sampling_rate_hz < MIN_SAMPLING_RATE) ||
	    (config->sampling_rate_hz > MAX_SAMPLING_RATE))
	{
		return false;
	}

	if ((config->log_level < LOG_LEVEL_OFF) ||
	    (config->log_level > LOG_LEVEL_DEBUG))
	{
		return false;
	}

	return true;
}

static void set_default_config(device_cfg_t *config)
{
	config->baud_rate = DEFAULT_BAUD_RATE;
	config->sampling_rate_hz = DEFAULT_SAMPLING_RATE;
	config->log_level = DEFAULT_LOG_LEVEL;
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

static void print_current_config(const device_cfg_t *config)
{
	printf("\nCurrent configuration:\n");
	printf("  baud_rate       = %d\n", config->baud_rate);
	printf("  sampling_rate   = %d Hz\n", config->sampling_rate_hz);
	printf("  log_level       = %d (%s)\n",
	       config->log_level,
	       log_level_to_string(config->log_level));
}

static int sync_config(device_cfg_t *config)
{
	if (msync(config, sizeof(*config), MS_SYNC) == -1)
	{
		perror("msync");
		return -1;
	}

	return 0;
}

static device_cfg_t *map_config_file(void)
{
	int file_descriptor;
	bool initialize_defaults;
	struct stat file_status;
	device_cfg_t *config;

	file_descriptor = open(CONFIG_FILE_PATH,
			       O_RDWR | O_CREAT,
			       CONFIG_FILE_PERMISSION);

	if (file_descriptor == -1)
	{
		perror("open");
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

	initialize_defaults =
		(file_status.st_size != (off_t)sizeof(device_cfg_t));

	if (ftruncate(file_descriptor, sizeof(device_cfg_t)) == -1)
	{
		perror("ftruncate");

		if (close(file_descriptor) == -1)
		{
			perror("close");
		}

		return NULL;
	}

	config = mmap(NULL,
		      sizeof(device_cfg_t),
		      PROT_READ | PROT_WRITE,
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

		if (munmap(config, sizeof(device_cfg_t)) == -1)
		{
			perror("munmap");
		}

		return NULL;
	}

	if (initialize_defaults || !is_valid_config(config))
	{
		set_default_config(config);

		if (sync_config(config) == -1)
		{
			if (munmap(config, sizeof(device_cfg_t)) == -1)
			{
				perror("munmap");
			}

			return NULL;
		}
	}

	return config;
}

static int update_baud_rate(device_cfg_t *config)
{
	char input[INPUT_BUFFER_SIZE];
	int baud_rate;
	int input_result;

	printf("Select baud rate [9600/115200/460800]: ");

	input_result = read_input(input, sizeof(input));
	if (input_result != 0)
	{
		return input_result;
	}

	if ((parse_integer(input, &baud_rate) == -1) ||
	    !is_supported_baud_rate(baud_rate))
	{
		fprintf(stderr, "Invalid baud rate.\n");
		return -1;
	}

	config->baud_rate = baud_rate;

	if (sync_config(config) == -1)
	{
		return -1;
	}

	printf("[Updated] baud_rate = %d\n", config->baud_rate);

	return 0;
}

static int update_sampling_rate(device_cfg_t *config)
{
	char input[INPUT_BUFFER_SIZE];
	int sampling_rate;
	int input_result;

	printf("Enter sampling rate [%d-%d Hz]: ",
	       MIN_SAMPLING_RATE,
	       MAX_SAMPLING_RATE);

	input_result = read_input(input, sizeof(input));
	if (input_result != 0)
	{
		return input_result;
	}

	if ((parse_integer(input, &sampling_rate) == -1) ||
	    (sampling_rate < MIN_SAMPLING_RATE) ||
	    (sampling_rate > MAX_SAMPLING_RATE))
	{
		fprintf(stderr,
			"Sampling rate must be between %d and %d Hz.\n",
			MIN_SAMPLING_RATE,
			MAX_SAMPLING_RATE);
		return -1;
	}

	config->sampling_rate_hz = sampling_rate;

	if (sync_config(config) == -1)
	{
		return -1;
	}

	printf("[Updated] sampling_rate_hz = %d\n",
	       config->sampling_rate_hz);

	return 0;
}

static int update_log_level(device_cfg_t *config)
{
	char input[INPUT_BUFFER_SIZE];
	int log_level;
	int input_result;

	printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");

	input_result = read_input(input, sizeof(input));
	if (input_result != 0)
	{
		return input_result;
	}

	if ((parse_integer(input, &log_level) == -1) ||
	    (log_level < LOG_LEVEL_OFF) ||
	    (log_level > LOG_LEVEL_DEBUG))
	{
		fprintf(stderr, "Invalid log level.\n");
		return -1;
	}

	config->log_level = log_level;

	if (sync_config(config) == -1)
	{
		return -1;
	}

	printf("[Updated] log_level = %d (%s)\n",
	       config->log_level,
	       log_level_to_string(config->log_level));

	return 0;
}

static int command_loop(device_cfg_t *config)
{
	char command[INPUT_BUFFER_SIZE];
	int input_result;
	int update_result;

	while (true)
	{
		print_current_config(config);

		printf("\nSelect field to update [baud/rate/log/quit]: ");

		input_result = read_input(command, sizeof(command));

		if (input_result == 1)
		{
			printf("\n");
			break;
		}

		if (input_result == -1)
		{
			continue;
		}

		if (strcmp(command, COMMAND_QUIT) == 0)
		{
			break;
		}

		if (strcmp(command, COMMAND_BAUD) == 0)
		{
			update_result = update_baud_rate(config);
		}
		else if (strcmp(command, COMMAND_RATE) == 0)
		{
			update_result = update_sampling_rate(config);
		}
		else if (strcmp(command, COMMAND_LOG) == 0)
		{
			update_result = update_log_level(config);
		}
		else
		{
			fprintf(stderr, "Unknown command: %s\n", command);
			continue;
		}

		if (update_result == 1)
		{
			printf("\n");
			break;
		}

		if (update_result == -1)
		{
			fprintf(stderr, "Configuration was not updated.\n");
		}
	}

	return 0;
}

int main(void)
{
	device_cfg_t *config;
	int exit_status = EXIT_SUCCESS;

	config = map_config_file();
	if (config == NULL)
	{
		return EXIT_FAILURE;
	}

	printf("[Config Writer] Loaded %s\n", CONFIG_FILE_PATH);

	command_loop(config);

	if (munmap(config, sizeof(device_cfg_t)) == -1)
	{
		perror("munmap");
		exit_status = EXIT_FAILURE;
	}

	return exit_status;
}