#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <string.h>
#include <time.h>

#define LOG_FILE	"system.log"
#define LOG_LEVEL	"INFO"
#define BUFFER_SIZE	256


static int write_full(int fd, const void *buf, size_t size)
{
	const char *ptr = buf;
	size_t total = 0;

	while (total < size) {
		ssize_t bytes;

		bytes = write(fd, ptr + total, size - total);

		if (bytes == -1)
			return -1;

		total += bytes;
	}

	return 0;
}


static int write_log(int fd, const char *message)
{
	char buffer[BUFFER_SIZE];
	char time_buf[32];
	time_t now;
	struct tm *tm_info;
	int length;


	now = time(NULL);

	tm_info = localtime(&now);
	if (tm_info == NULL) {
		fprintf(stderr, "localtime failed\n");
		return -1;
	}


	strftime(time_buf,
		 sizeof(time_buf),
		 "%Y-%m-%d %H:%M:%S",
		 tm_info);


	length = snprintf(buffer,
			  sizeof(buffer),
			  "[PID:%d] [%s] [%s] %s\n",
			  getpid(),
			  time_buf,
			  LOG_LEVEL,
			  message);


	if (length < 0 || length >= (int)sizeof(buffer)) {
		fprintf(stderr, "Log message too long\n");
		return -1;
	}


	if (write_full(fd, buffer, length) == -1) {
		perror("write");
		return -1;
	}


	return 0;
}


int main(int argc, char *argv[])
{
	int fd;
	int ret = EXIT_FAILURE;


	if (argc != 2) {
		fprintf(stderr,
			"Usage: %s \"message\"\n",
			argv[0]);
		return EXIT_FAILURE;
	}


	fd = open(LOG_FILE,
		  O_WRONLY | O_APPEND | O_CREAT,
		  0644);

	if (fd == -1) {
		perror("open");
		return EXIT_FAILURE;
	}


	if (flock(fd, LOCK_EX) == -1) {
		perror("flock lock");
		goto cleanup;
	}


	if (write_log(fd, argv[1]) == -1)
		goto unlock;


	ret = EXIT_SUCCESS;


unlock:
	if (flock(fd, LOCK_UN) == -1)
		perror("flock unlock");


cleanup:
	if (close(fd) == -1)
		perror("close");


	return ret;
}