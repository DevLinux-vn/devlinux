#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "iot_server.h"

static volatile sig_atomic_t g_stop = 0;
static client_t clients[MAX_CLIENTS];
static int    g_server_mode = 0;      
static double g_temp = 25.0;
static double g_hum  = 30.0;
static int    g_next_id = 1;

static void on_sigint(int sig)
{
    (void)sig;
    g_stop = 1;
}

/* Send the whole buffer, handling partial writes / EINTR / EAGAIN. */
static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                fd_set wfds;
                struct timeval tv = {1, 0};
                FD_ZERO(&wfds);
                FD_SET(fd, &wfds);
                int r = select(fd + 1, NULL, &wfds, NULL, &tv);
                if (r < 0 && errno == EINTR)
                    continue;
                if (r <= 0)
                    return -1;      
                continue;
            }
            perror("send");
            return -1;
        }
        sent += (size_t)n;
    }
    return 0;
}

static int count_clients(void)
{
    int n = 0;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].fd >= 0)
            n++;
    return n;
}

static void remove_client(client_t *c, fd_set *rfds)
{
    printf("[Server] Client %d disconnected\n", c->id);
    fflush(stdout);
    if (rfds)
        FD_CLR(c->fd, rfds);
    close(c->fd);
    c->fd = -1;
    c->inlen = 0;
}

static void trim(char *s)
{
    char *start = s;
    while (*start == ' ' || *start == '\t')
        start++;
    size_t len = strlen(start);
    while (len > 0 && (start[len - 1] == '\r' || start[len - 1] == ' ' ||
                       start[len - 1] == '\t'))
        start[--len] = '\0';
    if (start != s)
        memmove(s, start, len + 1);
}

static void reply(client_t *c, fd_set *rfds, const char *msg)
{
    if (send_all(c->fd, msg, strlen(msg)) < 0)
        remove_client(c, rfds);
}

static void handle_command(client_t *c, char *line, fd_set *rfds)
{
    char cmd[32] = {0}, arg[32] = {0};
    char resp[128];

    trim(line);
    if (line[0] == '\0')
        return;

    c->last_activity = time(NULL);
    int n = sscanf(line, "%31s %31s", cmd, arg);

    if (n >= 1 && strcasecmp(cmd, "GET_TEMP") == 0) {
        g_temp = 25.0 + (rand() % 10);
        snprintf(resp, sizeof(resp), "%.1f\n", g_temp);
        printf("[Server] Client %d: GET_TEMP → %.1f\n", c->id, g_temp);
        reply(c, rfds, resp);
    } else if (n >= 1 && strcasecmp(cmd, "GET_HUMIDITY") == 0) {
        g_hum = 30.0 + (rand() % 30);
        snprintf(resp, sizeof(resp), "%.1f\n", g_hum);
        printf("[Server] Client %d: GET_HUMIDITY → %.1f\n", c->id, g_hum);
        reply(c, rfds, resp);
    } else if (n >= 1 && strcasecmp(cmd, "SET_MODE") == 0) {
        char *end = NULL;
        long m = (n == 2) ? strtol(arg, &end, 10) : -1;
        if (n == 2 && end != arg && *end == '\0' && m >= 0 && m <= 2) {
            c->mode = (int)m;
            g_server_mode = (int)m;
            printf("[Server] Client %d: SET_MODE %ld → OK\n", c->id, m);
            reply(c, rfds, "OK\n");
        } else {
            printf("[Server] Client %d: %s → ERROR: Invalid mode\n", c->id, line);
            reply(c, rfds, "ERROR: Invalid mode (use 0, 1 or 2)\n");
        }
    } else if (n >= 1 && strcasecmp(cmd, "QUIT") == 0) {
        printf("[Server] Client %d: QUIT\n", c->id);
        remove_client(c, rfds);
        return;
    } else {
        printf("[Server] Client %d: %s → ERROR: Unknown command\n", c->id, line);
        reply(c, rfds, "ERROR: Unknown command\n");
    }
    fflush(stdout);
}


static void handle_readable(client_t *c, fd_set *rfds)
{
    ssize_t n = recv(c->fd, c->inbuf + c->inlen, sizeof(c->inbuf) - 1 - c->inlen, 0);
    if (n < 0) {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        perror("recv");
        remove_client(c, rfds);
        return;
    }
    if (n == 0) {                       
        remove_client(c, rfds);
        return;
    }

    c->inlen += (size_t)n;
    char *start = c->inbuf;
    char *end   = c->inbuf + c->inlen;
    char *nl;

    while ((nl = memchr(start, '\n', (size_t)(end - start))) != NULL) {
        *nl = '\0';
        handle_command(c, start, rfds);
        if (c->fd < 0)                 
            return;
        start = nl + 1;
    }

    size_t rem = (size_t)(end - start);
    memmove(c->inbuf, start, rem);
    c->inlen = rem;

    if (c->inlen >= sizeof(c->inbuf) - 1) {   
        c->inlen = 0;
        reply(c, rfds, "ERROR: Command too long\n");
    }
}

static void broadcast_status(void)
{
    int nclients = count_clients();
    if (nclients == 0)
        return;

    g_temp = 25.0 + (rand() % 10);
    g_hum  = 30.0 + (rand() % 30);

    char body[128], line[160];
    snprintf(body, sizeof(body), "Temp=%.1f Humidity=%.1f Mode=%d Clients=%d",
             g_temp, g_hum, g_server_mode, nclients);
    snprintf(line, sizeof(line), "[SERVER_STATUS] %s\n", body);

    printf("[Server] Broadcasting to %d clients: %s\n", nclients, body);
    fflush(stdout);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd < 0)
            continue;
        if (send_all(clients[i].fd, line, strlen(line)) < 0)
            remove_client(&clients[i], NULL);
    }
}

static void accept_client(int listen_fd)
{
    struct sockaddr_in ca;
    socklen_t clen = sizeof(ca);
    int cfd = accept(listen_fd, (struct sockaddr *)&ca, &clen);
    if (cfd < 0) {
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
            perror("accept");
        return;
    }

    int slot = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].fd < 0) { slot = i; break; }

    if (slot < 0) {
        const char *msg = "ERROR: Server full\n";
        send(cfd, msg, strlen(msg), MSG_NOSIGNAL);
        close(cfd);
        printf("[Server] Connection rejected: server full\n");
        return;
    }

    int flags = fcntl(cfd, F_GETFL, 0);
    if (flags < 0 || fcntl(cfd, F_SETFL, flags | O_NONBLOCK) < 0)
        perror("fcntl");

    client_t *c = &clients[slot];
    c->fd = cfd;
    c->id = g_next_id++;
    c->mode = 0;
    c->last_activity = time(NULL);
    c->inlen = 0;

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ca.sin_addr, ip, sizeof(ip));
    printf("[Server] Client %d connected from %s:%d\n", c->id, ip, ntohs(ca.sin_port));
    fflush(stdout);
}

int main(void)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].fd = -1;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;         
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    signal(SIGPIPE, SIG_IGN);
    srand((unsigned)time(NULL));

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        perror("setsockopt");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(listen_fd);
        return EXIT_FAILURE;
    }
    if (listen(listen_fd, 5) < 0) {
        perror("listen");
        close(listen_fd);
        return EXIT_FAILURE;
    }
    printf("[Server] Listening on localhost:%d\n", SERVER_PORT);
    fflush(stdout);

    time_t last_broadcast = time(NULL);

    while (!g_stop) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listen_fd, &readfds);
        int max_fd = listen_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd >= 0) {
                FD_SET(clients[i].fd, &readfds);
                if (clients[i].fd > max_fd)
                    max_fd = clients[i].fd;
            }
        }

        struct timeval tv = {1, 0};     
        int ready = select(max_fd + 1, &readfds, NULL, NULL, &tv);
        if (ready < 0) {
            if (errno == EINTR)
                continue;              
            perror("select");
            break;
        }

        if (ready > 0) {
            if (FD_ISSET(listen_fd, &readfds))
                accept_client(listen_fd);

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd >= 0 && FD_ISSET(clients[i].fd, &readfds))
                    handle_readable(&clients[i], &readfds);
            }
        }

        time_t now = time(NULL);
        if (now - last_broadcast >= BROADCAST_INTERVAL) {
            broadcast_status();
            last_broadcast = now;
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd >= 0) {
            close(clients[i].fd);
            clients[i].fd = -1;
        }
    }
    close(listen_fd);
    printf("\n[Server] Shutdown complete.\n");
    return EXIT_SUCCESS;
}