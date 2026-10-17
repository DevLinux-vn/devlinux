#include <stdio.h>
#include <time.h>

#include "logger.h"

#define LOG_FILE "app.log"

void log_write(const char *msg)
{
	FILE *f = fopen(LOG_FILE, "a");

	if (f == NULL) {
		perror("fopen failed");
		return;
	}

	fprintf(f, "%s\n", msg);

	if (fclose(f) != 0) {
		perror("fclose failed");
	}
}

void log_timestamp(void)
{
	FILE *f = fopen(LOG_FILE, "a");

	if (f == NULL) {
		perror("fopen failed");
		return;
	}

	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	if (t == NULL) {
		perror("localtime failed");
		fclose(f);
		return;
	}

	fprintf(f,
		"%04d-%02d-%02d %02d:%02d:%02d\n",
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);

	if (fclose(f) != 0) {
		perror("fclose failed");
	}
}

void log_error(const char *msg)
{
	FILE *f = fopen(LOG_FILE, "a");

	if (f == NULL) {
		perror("fopen failed");
		return;
	}

	fprintf(f, "[ERROR] %s\n", msg);

	if (fclose(f) != 0) {
		perror("fclose failed");
	}
}