#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main(void)
{
	int fd;
	struct sockaddr_un addr;

	char command[BUFFER_SIZE];
	char response[BUFFER_SIZE];

	while (1)
	{
		printf("> ");

		if (fgets(command,
			  sizeof(command),
			  stdin) == NULL)
		{
			break;
		}

		command[strcspn(command, "\n")] = '\0';

		fd = socket(AF_UNIX,
			    SOCK_STREAM,
			    0);

		if (fd < 0)
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

		if (connect(fd,
			    (struct sockaddr *)&addr,
			    sizeof(addr))
		    < 0)
		{
			perror("connect");
			close(fd);
			return 1;
		}

		ssize_t n;

		n = write(fd,
			  command,
			  strlen(command));

		if (n < 0)
		{
			perror("write");
			close(fd);
			return 1;
		}

		n = read(fd,
			 response,
			 sizeof(response) - 1);

		if (n <= 0)
		{
			perror("read");
			close(fd);
			return 1;
		}

		response[n] = '\0';

		printf("%s\n",
		       response);

		close(fd);

		if (strcmp(command,
			   "quit")
		    == 0)
		{
			break;
		}
	}

	return 0;
}
