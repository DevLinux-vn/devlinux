/* SSD1306 demo and optional raw bitmap upload through the character device. */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "../driver/ssd1306.h"

#define DEVICE_PATH "/dev/ssd1306_i2c"
#define DEMO_DELAY_SECONDS 3
#define STATUS_SIZE 256

static volatile sig_atomic_t stop_requested;

/* sigaction requires a void callback; the handler only sets a signal-safe flag. */
static void request_stop(int signal_number)
{
	(void)signal_number;
	stop_requested = 1;
}

static int load_bitmap(const char *path, struct ssd1306_bitmap *bitmap)
{
	struct stat info;
	size_t offset = 0;
	ssize_t count;
	unsigned char extra;
	int saved_errno;
	int result = -1;
	int fd = open(path, O_RDONLY | O_CLOEXEC | O_NONBLOCK);

	if (fd < 0)
		return -1;
	/* Reject streams before reading: a bitmap is a finite regular file. */
	if (fstat(fd, &info) < 0)
		goto out;
	if (!S_ISREG(info.st_mode) || info.st_size != sizeof(bitmap->pixels)) {
		errno = EINVAL;
		goto out;
	}
	while (offset < sizeof(bitmap->pixels)) {
		count = read(fd, bitmap->pixels + offset, sizeof(bitmap->pixels) - offset);
		if (count < 0 && errno == EINTR && !stop_requested)
			continue;
		if (count <= 0) {
			if (!count)
				errno = EINVAL;
			goto out;
		}
		offset += (size_t)count;
	}
	do {
		count = read(fd, &extra, sizeof(extra));
	} while (count < 0 && errno == EINTR && !stop_requested);
	if (count != 0) {
		if (count > 0)
			errno = EINVAL;
		goto out;
	}
	result = 0;
out:
	saved_errno = errno;
	if (close(fd) < 0 && result == 0)
		return -1;
	errno = saved_errno;
	return result;
}

static int make_border(struct ssd1306_bitmap *bitmap)
{
	unsigned int x;
	unsigned int page;

	memset(bitmap, 0, sizeof(*bitmap));
	for (x = 0; x < SSD1306_WIDTH; x++) {
		bitmap->pixels[x] |= 1;
		bitmap->pixels[(SSD1306_PAGES - 1) * SSD1306_WIDTH + x] |= 0x80;
	}
	for (page = 0; page < SSD1306_PAGES; page++) {
		bitmap->pixels[page * SSD1306_WIDTH] = 0xff;
		bitmap->pixels[page * SSD1306_WIDTH + SSD1306_WIDTH - 1] = 0xff;
	}
	return 0;
}

static int send_command(int fd, const char *command)
{
	size_t length = strlen(command);
	ssize_t written;

	do {
		written = write(fd, command, length);
	} while (written < 0 && errno == EINTR && !stop_requested);
	if (written < 0)
		return -1;
	/* Commands are atomic records: never resend a positive partial result. */
	if ((size_t)written != length) {
		errno = EIO;
		return -1;
	}
	return 0;
}

static int print_status(int fd)
{
	char status[STATUS_SIZE];
	size_t offset = 0;
	ssize_t count;

	while (offset < sizeof(status) - 1) {
		count = read(fd, status + offset, sizeof(status) - 1 - offset);
		if (count < 0 && errno == EINTR && !stop_requested)
			continue;
		if (count < 0)
			return -1;
		if (!count)
			break;
		offset += (size_t)count;
	}
	if (!offset || offset == sizeof(status) - 1) {
		errno = EIO;
		return -1;
	}
	status[offset] = '\0';
	return printf("[+] %s", status) < 0 ? -1 : 0;
}

int main(int argc, char **argv)
{
	static const char *const texts[] = {
		"TEXT Hello World", "TEXT Raspberry Pi", "TEXT SSD1306 Test",
	};
	struct sigaction action = { .sa_handler = request_stop };
	struct sigaction ignore_pipe = { .sa_handler = SIG_IGN };
	struct timespec delay = { .tv_sec = DEMO_DELAY_SECONDS };
	struct ssd1306_bitmap bitmap;
	int fd = -1;
	int result = EXIT_FAILURE;
	int display_on = 0;
	int ret;
	size_t i;

	if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
		perror("setvbuf");
		return EXIT_FAILURE;
	}
	if (argc != 1 && (argc != 3 || strcmp(argv[1], "--bitmap"))) {
		if (fprintf(stderr, "Usage: %s [--bitmap raw-1024-byte-file]\n", argv[0]) < 0)
			perror("print usage");
		return EXIT_FAILURE;
	}
	if (sigemptyset(&action.sa_mask) < 0 || sigemptyset(&ignore_pipe.sa_mask) < 0 ||
	    sigaction(SIGINT, &action, NULL) < 0 || sigaction(SIGTERM, &action, NULL) < 0 ||
	    sigaction(SIGPIPE, &ignore_pipe, NULL) < 0) {
		perror("sigaction");
		return EXIT_FAILURE;
	}
	if ((argc == 3 ? load_bitmap(argv[2], &bitmap) : make_border(&bitmap)) < 0) {
		perror("prepare bitmap (exactly 1024 bytes required)");
		goto cleanup;
	}
	if (stop_requested) {
		result = EXIT_SUCCESS;
		goto cleanup;
	}
	fd = open(DEVICE_PATH, O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		perror("open " DEVICE_PATH);
		goto cleanup;
	}
	if (printf("[+] Opened " DEVICE_PATH "\n") < 0 || send_command(fd, "CLEAR") < 0 ||
	    printf("[+] Cleared display\n") < 0) {
		perror("clear display");
		goto cleanup;
	}
	do {
		ret = ioctl(fd, SSD1306_IOC_UPLOAD_BITMAP, &bitmap);
	} while (ret < 0 && errno == EINTR && !stop_requested);
	if (ret < 0 || printf("[+] Bitmap uploaded (%zu bytes)\n", sizeof(bitmap.pixels)) < 0) {
		perror("upload bitmap");
		goto cleanup;
	}
	for (i = 0; i < sizeof(texts) / sizeof(texts[0]); i++) {
		if (stop_requested) {
			result = EXIT_SUCCESS;
			goto cleanup;
		}
		if (send_command(fd, texts[i]) < 0 ||
		    printf("[+] Rendered text: %s\n", texts[i] + sizeof("TEXT ") - 1) < 0) {
			perror("render text");
			goto cleanup;
		}
	}
	if (send_command(fd, "ON") < 0) {
		perror("turn display on");
		goto cleanup;
	}
	display_on = 1;
	if (printf("[+] Display turned ON\n[+] Waiting 3 seconds...\n") < 0) {
		perror("print display state");
		goto cleanup;
	}
	while (!stop_requested && nanosleep(&delay, &delay) < 0) {
		if (errno != EINTR) {
			perror("nanosleep");
			goto cleanup;
		}
	}
	if (send_command(fd, "OFF") < 0) {
		perror("turn display off");
		goto cleanup;
	}
	display_on = 0;
	if (printf("[+] Display turned OFF\n") < 0 || print_status(fd) < 0) {
		perror("read/print display status");
		goto cleanup;
	}
	result = EXIT_SUCCESS;

cleanup:
	if (display_on && send_command(fd, "OFF") < 0) {
		perror("cleanup display OFF");
		result = EXIT_FAILURE;
	}
	if (fd >= 0 && close(fd) < 0) {
		perror("close display");
		result = EXIT_FAILURE;
	}
	if (result == EXIT_SUCCESS && printf("[+] Application completed successfully\n") < 0)
		result = EXIT_FAILURE;
	if (fflush(stdout) == EOF) {
		perror("flush stdout");
		result = EXIT_FAILURE;
	}
	return result;
}
