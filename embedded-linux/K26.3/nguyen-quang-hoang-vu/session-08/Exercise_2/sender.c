#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9000
#define BUFFER_SIZE 256

int main(void)
{
	int sockfd;

	struct sockaddr_in server_addr;

	sockfd = socket(AF_INET,
			SOCK_DGRAM,
			0);

	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	memset(&server_addr,
	       0,
	       sizeof(server_addr));

	server_addr.sin_family = AF_INET;

	server_addr.sin_port = htons(PORT);

	server_addr.sin_addr.s_addr =
		inet_addr("127.0.0.1");

	if (server_addr.sin_addr.s_addr == INADDR_NONE)
	{
		fprintf(stderr,
			"Invalid IP address\n");

		close(sockfd);

		return 1;
	}

	for (int i = 0; i < 5; i++)
	{
		FILE *fp;

		float load1 = 0.0f;
		float temp;

		long mem_total = 0;
		long mem_free = 0;

		float mem_used_percent;

		char line[BUFFER_SIZE];

		char message[BUFFER_SIZE];

		fp = fopen("/proc/loadavg",
			   "r");

		if (fp != NULL)
		{
			fscanf(fp,
			       "%f",
			       &load1);

			fclose(fp);
		}

		fp = fopen("/proc/meminfo",
			   "r");

		if (fp != NULL)
		{
			while (fgets(line,
				     sizeof(line),
				     fp))
			{
				if (sscanf(line,
					   "MemTotal: %ld",
					   &mem_total) == 1)
				{
					continue;
				}

				if (sscanf(line,
					   "MemFree: %ld",
					   &mem_free) == 1)
				{
					continue;
				}
			}

			fclose(fp);
		}

		temp = 40.0f +
		       load1 * 10.0f;

		mem_used_percent =
			100.0f *
			((float)(mem_total - mem_free))
			/ mem_total;

		snprintf(message,
			 sizeof(message),
			 "id=sensor-01 temp=%.1f mem_used=%.1f%%",
			 temp,
			 mem_used_percent);

		ssize_t sent;

		sent = sendto(sockfd,
			      message,
			      strlen(message),
			      0,
			      (struct sockaddr *)&server_addr,
			      sizeof(server_addr));

		if (sent < 0)
		{
			perror("sendto");

			close(sockfd);

			return 1;
		}

		printf("[SENT %d/5] %s\n",
		       i + 1,
		       message);

		sleep(2);
	}

	close(sockfd);

	return 0;
}
