#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9000
#define BUFFER_SIZE 256

int main(void)
{
	int sockfd;

	int reuse = 1;

	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	socklen_t client_len =
		sizeof(client_addr);

	char buffer[BUFFER_SIZE];

	sockfd = socket(AF_INET,
			SOCK_DGRAM,
			0);

	if (sockfd < 0)
	{
		perror("socket");
		return 1;
	}

	if (setsockopt(sockfd,
		       SOL_SOCKET,
		       SO_REUSEADDR,
		       &reuse,
		       sizeof(reuse))
	    < 0)
	{
		perror("setsockopt");
	}

	memset(&server_addr,
	       0,
	       sizeof(server_addr));

	server_addr.sin_family =
		AF_INET;

	server_addr.sin_addr.s_addr =
		INADDR_ANY;

	server_addr.sin_port =
		htons(PORT);

	if (bind(sockfd,
		 (struct sockaddr *)&server_addr,
		 sizeof(server_addr))
	    < 0)
	{
		perror("bind");

		close(sockfd);

		return 1;
	}

	printf("[RECEIVER] Listening on port %d\n",
	       PORT);

	while (1)
	{
		memset(buffer,
		       0,
		       sizeof(buffer));

		ssize_t n;

		n = recvfrom(sockfd,
			     buffer,
			     sizeof(buffer) - 1,
			     0,
			     (struct sockaddr *)&client_addr,
			     &client_len);

		if (n < 0)
		{
			perror("recvfrom");
			continue;
		}

		buffer[n] = '\0';

		char ip[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			  &client_addr.sin_addr,
			  ip,
			  sizeof(ip));

		time_t now;

		now = time(NULL);

		struct tm *tm_now;

		tm_now = localtime(&now);

		char timestamp[16];

		strftime(timestamp,
			 sizeof(timestamp),
			 "%H:%M:%S",
			 tm_now);

		printf("[%s] %s:%d -> %s\n",
		       timestamp,
		       ip,
		       ntohs(client_addr.sin_port),
		       buffer);
	}

	close(sockfd);

	return 0;
}
