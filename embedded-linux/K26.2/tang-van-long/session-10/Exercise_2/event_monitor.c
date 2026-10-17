#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "event_monitor.h"

#define MAX_CLIENTS 8

typedef struct {
    int active; 
} client_meta_t;

static volatile sig_atomic_t g_shutdown = 0;

static int g_fifo_fd = -1;
static int g_sock_fd = -1;
static int g_file_fd = -1;

static off_t g_last_size = 0;
static long  g_event_count = 0;
static int   g_monitoring_active = 1; 
static time_t g_start_time;

static void handle_sigterm(int signo)
{
    (void)signo;
    g_shutdown = 1;
}

static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            perror("send");
            return -1;
        }
        sent += (size_t)n;
    }
    return 0;
}

static void trim_newline(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
        s[len - 1] = '\0';
        len--;
    }
}

static int setup_fifo(void)
{
    if (mkfifo(FIFO_PATH, 0666) < 0 && errno != EEXIST) {
        perror("mkfifo");
        return -1;
    }
    
    int fd = open(FIFO_PATH, O_RDWR | O_NONBLOCK);
    if (fd < 0) {
        perror("open fifo");
        return -1;
    }
    return fd;
}

static int setup_socket(void)
{
    struct sockaddr_un addr;
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    unlink(SOCKET_PATH); 

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, MAX_CLIENTS) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}

static int setup_file(void)
{
    int fd = open(FILE_PATH, O_RDONLY | O_CREAT, 0644);
    if (fd < 0) {
        perror("open file");
        return -1;
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return -1;
    }
    g_last_size = st.st_size;
    return fd;
}

static void handle_fifo_readable(void)
{
    char buf[BUF_SIZE];
    ssize_t n = read(g_fifo_fd, buf, sizeof(buf) - 1);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
            return;
        perror("read fifo");
        return;
    }
    if (n == 0)
        return; 

    buf[n] = '\0';

    char *saveptr = NULL;
    char *line = strtok_r(buf, "\n", &saveptr);
    while (line != NULL) {
        if (g_monitoring_active) {
            printf("[FIFO_EVENT] %s\n", line);
            g_event_count++;
        }
        line = strtok_r(NULL, "\n", &saveptr);
    }
}

static void handle_file_check(void)
{
    struct stat st;
    if (stat(FILE_PATH, &st) < 0) {
        return;
    }
    if (st.st_size != g_last_size) {
        if (g_monitoring_active) {
            printf("[FILE_EVENT] %s size changed to %lld bytes\n",
                   FILE_PATH, (long long)st.st_size);
            g_event_count++;
        }
        g_last_size = st.st_size;
    }
}

static int handle_client_command(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t n = recv(cfd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) {
        if (n < 0 && errno != ECONNRESET)
            perror("recv");
        return -1;
    }
    buf[n] = '\0';
    trim_newline(buf);

    char response[BUF_SIZE];

    if (strcmp(buf, "STATUS") == 0) {
        long uptime = (long)difftime(time(NULL), g_start_time);
        int len = snprintf(response, sizeof(response),
                            "Total events: %ld, Uptime: %ld seconds\n",
                            g_event_count, uptime);
        send_all(cfd, response, (size_t)len);
        return 0;

    } else if (strcmp(buf, "START") == 0) {
        g_monitoring_active = 1;
        send_all(cfd, "OK\n", 3);
        return 0;

    } else if (strcmp(buf, "STOP") == 0) {
        g_monitoring_active = 0;
        send_all(cfd, "OK\n", 3);
        return 0;

    } else if (strcmp(buf, "EXIT") == 0) {
        return -1; 

    } else {
        send_all(cfd, "ERROR: Unknown command\n", 24);
        return 0;
    }
}

static void cleanup_and_exit(int sock_fds[], int n_clients)
{
    for (int i = 0; i < n_clients; i++) {
        if (sock_fds[i] != -1)
            close(sock_fds[i]);
    }
    if (g_fifo_fd != -1)
        close(g_fifo_fd);
    if (g_sock_fd != -1)
        close(g_sock_fd);
    if (g_file_fd != -1)
        close(g_file_fd);
    unlink(SOCKET_PATH);
    printf("[Monitor] Shutdown complete.\n");
}

int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL); 
    g_fifo_fd = setup_fifo();
    g_sock_fd = setup_socket();
    g_file_fd = setup_file();

    if (g_fifo_fd < 0 || g_sock_fd < 0 || g_file_fd < 0) {
        fprintf(stderr, "[Monitor] Initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    g_start_time = time(NULL);

    printf("[Monitor] Listening on %s\n", SOCKET_PATH);
    printf("[Monitor] Monitoring %s (FIFO) and %s\n", FIFO_PATH, FILE_PATH);

    int client_fds[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++)
        client_fds[i] = -1;

    struct pollfd fds[NUM_POLL_FDS + MAX_CLIENTS];

    while (!g_shutdown) {
        fds[FD_FIFO].fd = g_fifo_fd;
        fds[FD_FIFO].events = POLLIN;
        fds[FD_FIFO].revents = 0;

        fds[FD_SOCKET_LISTENER].fd = g_sock_fd;
        fds[FD_SOCKET_LISTENER].events = POLLIN;
        fds[FD_SOCKET_LISTENER].revents = 0;

        fds[FD_FILE].fd = g_file_fd;
        fds[FD_FILE].events = POLLERR; 
        fds[FD_FILE].revents = 0;

        nfds_t nfds = NUM_POLL_FDS;
        int slot_for_index[MAX_CLIENTS]; 
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != -1) {
                fds[nfds].fd = client_fds[i];
                fds[nfds].events = POLLIN;
                fds[nfds].revents = 0;
                slot_for_index[nfds] = i;
                nfds++;
            }
        }

        int ret = poll(fds, nfds, POLL_TIMEOUT_MS);

        if (ret < 0) {
            if (errno == EINTR)
                continue; 
            perror("poll");
            break;
        }

        if (ret == 0) {
            printf("[HEARTBEAT] Monitor alive, events_seen=%ld\n", g_event_count);
        } else {
            if (fds[FD_FIFO].revents & POLLIN)
                handle_fifo_readable();

            if (fds[FD_SOCKET_LISTENER].revents & POLLIN) {
                int cfd = accept(g_sock_fd, NULL, NULL);
                if (cfd < 0) {
                    perror("accept");
                } else {
                    int placed = 0;
                    for (int i = 0; i < MAX_CLIENTS; i++) {
                        if (client_fds[i] == -1) {
                            client_fds[i] = cfd;
                            placed = 1;
                            break;
                        }
                    }
                    if (!placed) {
                        fprintf(stderr, "[Monitor] Too many control clients, rejecting\n");
                        close(cfd);
                    }
                }
            }

            for (nfds_t i = NUM_POLL_FDS; i < nfds; i++) {
                if (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
                    int idx = slot_for_index[i];
                    if (handle_client_command(client_fds[idx]) == -1) {
                        close(client_fds[idx]);
                        client_fds[idx] = -1;
                    }
                }
            }
        }

        handle_file_check();
    }

    cleanup_and_exit(client_fds, MAX_CLIENTS);
    return 0;
}