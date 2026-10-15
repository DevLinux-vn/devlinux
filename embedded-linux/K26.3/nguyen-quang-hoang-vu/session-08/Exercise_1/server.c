#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

void signal_handler(int sig)
{
	(void)sig;

	unlink(SOCKET_PATH);

	exit(0);
}

int main(void)
{
	int server_fd;
	int client_fd;

	struct sockaddr_un addr;

	signal(SIGINT,
	       signal_handler);

	unlink(SOCKET_PATH);

	server_fd = socket(AF_UNIX,
			   SOCK_STREAM,
			   0);

	if (server_fd < 0)
	{
		perror("socket");
		return 1;
	}

	memset(&addr,
	       0,
	       sizeof(addr));

	addr.sun_family = AF_UNIX;

	strncpy(addr.sun_path,
		SOCKET_PATH,
		sizeof(addr.sun_path) - 1);

	if (bind(server_fd,
		 (struct sockaddr *)&addr,
		 sizeof(addr))
	    < 0)
	{
		perror("bind");
		return 1;
	}

	if (listen(server_fd,
		   5)
	    < 0)
	{
		perror("listen");
		return 1;
	}

	printf("[SERVER] Listening...\n");

	while (1)
	{
		char buffer[BUFFER_SIZE];

		client_fd =
			accept(server_fd,
			       NULL,
			       NULL);

		if (client_fd < 0)
		{
			perror("accept");
			continue;
		}

		ssize_t n;

		n = read(client_fd,
			 buffer,
			 sizeof(buffer) - 1);

		if (n <= 0)
		{
			close(client_fd);
			continue;
		}

		buffer[n] = '\0';

		if (strcmp(buffer,
			   "cpu")
		    == 0)
		{
			FILE *fp;

			float load1;
			float load5;
			float load15;

			fp = fopen("/proc/loadavg",
				   "r");

			if (fp != NULL)
			{
				fscanf(fp,
				       "%f %f %f",
				       &load1,
				       &load5,
				       &load15);

				fclose(fp);

				snprintf(buffer,
					 sizeof(buffer),
					 "load_avg=%.2f",
					 load1);
			}
		}
		else if (strcmp(buffer,
				"mem")
			 == 0)
		{
			FILE *fp;

			char line[BUFFER_SIZE];

			long mem_total = 0;
			long mem_free = 0;

			fp = fopen("/proc/meminfo",
				   "r");

			if (fp != NULL)
			{
				while (fgets(line,
					     sizeof(line),
					     fp))
				{
					sscanf(line,
					       "MemTotal: %ld",
					       &mem_total);

					sscanf(line,
					       "MemFree: %ld",
					       &mem_free);
				}

				fclose(fp);

				snprintf(buffer,
					 sizeof(buffer),
					 "mem_total=%ld mem_free=%ld",
					 mem_total,
					 mem_free);
			}
		}
		else if (strcmp(buffer,
				"quit")
			 == 0)
		{
			strcpy(buffer,
			       "Bye");
		}
		else
		{
			strcpy(buffer,
			       "Unknown command");
		}

		n = write(client_fd,
			  buffer,
			  strlen(buffer));

		if (n < 0)
		{
			perror("write");
		}

		close(client_fd);
	}
}
