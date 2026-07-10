#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CLIENTS 100

typedef struct {
	int fd;
	char username[64];
	int authenticated;
	char input_buffer[4096];
	int input_len;
	char output_buffer[4096];
	int output_len;
	int output_pos;
} client_t;

extern client_t clients[MAX_CLIENTS];

void send_message(client_t *client, const char *text);
void save_message_log(const char *username, const char *text);

void broadcast_message(const char *username, const char *text)
{
	int i;
	char buf[512];

	snprintf(buf, sizeof(buf), "FROM:%s:%s\n", username, text);

	for (i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].fd >= 0 && clients[i].authenticated) {
			send_message(&clients[i], buf);
		}
	}

	save_message_log(username, text);
}
