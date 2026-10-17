#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "sensor_shm.h"

#define DAEMON_POLL_INTERVAL 2

static int shmid = -1;
static sensor_data_t *shm_ptr = NULL;

static void cleanup(int sig)
{
	const char msg[] = "\n[Daemon] Cleaning up shared memory. Goodbye.\n";

	(void)sig;

	write(STDOUT_FILENO, msg, sizeof(msg) - 1);

	if (shm_ptr != NULL)
		shmdt(shm_ptr);

	if (shmid != -1)
		shmctl(shmid, IPC_RMID, NULL);

	_exit(0);
}

static double read_cpu_temp(void)
{
	FILE *fp;
	double load1;

	fp = fopen("/proc/loadavg", "r");
	if (fp == NULL) {
		perror("fopen /proc/loadavg");
		return 40.0;
	}

	load1 = 0.0;

	if (fscanf(fp, "%lf", &load1) != 1) {
		fprintf(stderr, "Warning: failed to parse /proc/loadavg\n");
		fclose(fp);
		return 40.0;
	}

	fclose(fp);

	return 40.0 + load1 * 10.0;
}

static double read_ram_used_pct(void)
{
	FILE *fp;
	char key[64];
	char unit[32];
	long value;
	long mem_total;
	long mem_free;

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL) {
		perror("fopen /proc/meminfo");
		return 0.0;
	}

	mem_total = 0;
	mem_free = -1;

	while (fscanf(fp, "%63s %ld %31s", key, &value, unit) == 3) {
		if (strcmp(key, "MemTotal:") == 0)
			mem_total = value;
		else if (strcmp(key, "MemFree:") == 0)
			mem_free = value;

		if (mem_total > 0 && mem_free >= 0)
			break;
	}

	fclose(fp);

	if (mem_total <= 0) {
		fprintf(stderr, "Error: MemTotal not found or invalid\n");
		return 0.0;
	}

	if (mem_free < 0) {
		fprintf(stderr, "Error: MemFree not found or invalid\n");
		return 0.0;
	}

	return ((double)(mem_total - mem_free) / mem_total) * 100.0;
}

int main(void)
{
	if (signal(SIGINT, cleanup) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	shmid = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
	if (shmid == -1) {
		perror("shmget");
		exit(1);
	}

	shm_ptr = shmat(shmid, NULL, 0);
	if (shm_ptr == (void *)-1) {
		perror("shmat");
		shmctl(shmid, IPC_RMID, NULL);
		exit(1);
	}

	printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

	while (1) {
		shm_ptr->timestamp = time(NULL);
		shm_ptr->cpu_temp = read_cpu_temp();
		shm_ptr->ram_used_pct = read_ram_used_pct();

		printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n",
		       shm_ptr->cpu_temp,
		       shm_ptr->ram_used_pct);

		sleep(DAEMON_POLL_INTERVAL);
	}

	return 0;
}