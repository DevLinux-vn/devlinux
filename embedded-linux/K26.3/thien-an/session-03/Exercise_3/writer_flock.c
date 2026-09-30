/*
| Property              | flock                     | fcntl                            |
|-----------------------|---------------------------|----------------------------------|
| Lock granularity      | Whole file only           | Byte range supported             |
| Works over NFS        | No                        | Yes                              |
| Inherited across fork | Yes                       | No                               |
| Best used when        | Simple local file locking | Network FS or byte-range locking |
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h> 
#include <time.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s \"your message here\"\n", argv[0]);
		return 1;
	}

	int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd < 0) {
		perror("Error opening log file");
		return 1;
	}

	if (flock(fd, LOCK_EX) == -1) {
		perror("Error locking file");
		close(fd);
		return 1;
	}

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	char time_str[64];
	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);

	char buffer[512];
	int len = snprintf(buffer, sizeof(buffer), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

	if (len < 0 || len >= (int)sizeof(buffer)) {
		fprintf(stderr, "snprintf error\n");
		flock(fd, LOCK_UN);
		close(fd);
		return 1;
	}

	if (write(fd, buffer, len) == -1) {
		perror("Error writing to file");
	}

	if (flock(fd, LOCK_UN) == -1) {
		perror("Error unlocking file");
	}

	close(fd);
	return 0;
}