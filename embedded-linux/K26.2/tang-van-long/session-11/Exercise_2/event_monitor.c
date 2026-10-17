#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "event_monitor.h"

#define TOTAL_FDS (NUM_POLL_FDS + MAX_CTRL_CLIENTS)

static volatile sig_atomic_t g_stop = 0;
static int           g_active = 1;        
static unsigned long g_events = 0;
static time_t        g_start;
static off_t         g_last_size = 0;

static struct pollfd fds[TOTAL_FDS];
static char   ctrl_buf[MAX_CTRL_CLIENTS][LINE_BUF_SIZE];
static size_t ctrl_len[MAX_CTRL_CLIENTS];

static char   fifo_buf[LINE_BUF_SIZE];
static size_t fifo_len = 0;

static void on_signal(int sig)
{
    (void)sig;
    g_stop = 1;
}

static int send_all(int fd, const char *buf, size_t len)
{
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, MSG_NOSIGNAL);
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

static void trim(char *s)
{
    char *p = s;
    while (*p == ' ' || *p == '\t')
        p++;
    size_t len = strlen(p);
    while (len > 0 && (p[len - 1] == '\r' || p[len - 1] == ' ' || p[len - 1] == '\t'))
        p[--len] = '\0';
    if (p != s)
        memmove(s, p, len + 1);
}



static int setup_fifo(void)
{
    struct stat st;
    if (mkfifo(FIFO_PATH, 0666) < 0) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return -1;
        }
        if (stat(FIFO_PATH, &st) < 0 || !S_ISFIFO(st.st_mode)) {
            fprintf(stderr, "%s exists but is not a FIFO\n", FIFO_PATH);
            return -1;
        }
    }
    int fd = open(FIFO_PATH, O_RDWR | O_NONBLOCK);
    if (fd < 0)
        perror("open fifo");
    return fd;
}

static int setup_socket(void)
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);                
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }
    if (listen(fd, 5) < 0) {
        perror("listen");
        close(fd);
        unlink(SOCKET_PATH);
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
    if (stat(FILE_PATH, &st) < 0) {
        perror("stat");
        close(fd);
        return -1;
    }
    g_last_size = st.st_size;
    return fd;
}


static void handle_fifo(int fd)
{
    for (;;) {
        ssize_t n = read(fd, fifo_buf + fifo_len, sizeof(fifo_buf) - 1 - fifo_len);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            if (errno != EAGAIN && errno != EWOULDBLOCK)
                perror("read fifo");
            break;
        }
        if (n == 0)
            break;
        fifo_len += (size_t)n;

        char *start = fifo_buf, *nl;
        char *end = fifo_buf + fifo_len;
        while ((nl = memchr(start, '\n', (size_t)(end - start))) != NULL) {
            *nl = '\0';
            trim(start);
            if (start[0] != '\0' && g_active) {
                printf("[FIFO_EVENT] %s\n", start);
                g_events++;
            }
            start = nl + 1;
        }
        fifo_len = (size_t)(end - start);
        memmove(fifo_buf, start, fifo_len);

        if (fifo_len >= sizeof(fifo_buf) - 1) {    
            fifo_buf[fifo_len] = '\0';
            if (g_active) {
                printf("[FIFO_EVENT] %s\n", fifo_buf);
                g_events++;
            }
            fifo_len = 0;
        }
    }
    fflush(stdout);
}

static void close_ctrl(int idx)
{
    close(fds[NUM_POLL_FDS + idx].fd);
    fds[NUM_POLL_FDS + idx].fd = -1;
    ctrl_len[idx] = 0;
}

static void accept_ctrl(int listen_fd)
{
    int cfd = accept(listen_fd, NULL, NULL);
    if (cfd < 0) {
        if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
            perror("accept");
        return;
    }
    for (int i = 0; i < MAX_CTRL_CLIENTS; i++) {
        if (fds[NUM_POLL_FDS + i].fd < 0) {
            fds[NUM_POLL_FDS + i].fd = cfd;
            fds[NUM_POLL_FDS + i].events = POLLIN;
            ctrl_len[i] = 0;
            return;
        }
    }
    send_all(cfd, "ERROR: Too many clients\n", 24);
    close(cfd);
}

static void run_command(int idx, char *cmd)
{
    int fd = fds[NUM_POLL_FDS + idx].fd;
    char resp[128];

    trim(cmd);
    if (cmd[0] == '\0')
        return;

    if (strcasecmp(cmd, "STATUS") == 0) {
        snprintf(resp, sizeof(resp), "Total events: %lu, Uptime: %ld seconds\n",
                 g_events, (long)(time(NULL) - g_start));
        if (send_all(fd, resp, strlen(resp)) < 0)
            close_ctrl(idx);
    } else if (strcasecmp(cmd, "START") == 0) {
        g_active = 1;
        if (send_all(fd, "OK\n", 3) < 0)
            close_ctrl(idx);
    } else if (strcasecmp(cmd, "STOP") == 0) {
        g_active = 0;
        if (send_all(fd, "OK\n", 3) < 0)
            close_ctrl(idx);
    } else if (strcasecmp(cmd, "EXIT") == 0) {
        close_ctrl(idx);
    } else {
        if (send_all(fd, "ERROR: Unknown command\n", 23) < 0)
            close_ctrl(idx);
    }
}

static void handle_ctrl(int idx)
{
    int fd = fds[NUM_POLL_FDS + idx].fd;
    ssize_t n = recv(fd, ctrl_buf[idx] + ctrl_len[idx],
                     sizeof(ctrl_buf[idx]) - 1 - ctrl_len[idx], 0);
    if (n < 0) {
        if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        perror("recv");
        close_ctrl(idx);
        return;
    }
    if (n == 0) {                      
        close_ctrl(idx);
        return;
    }
    ctrl_len[idx] += (size_t)n;

    char *start = ctrl_buf[idx], *nl;
    char *end = ctrl_buf[idx] + ctrl_len[idx];
    while ((nl = memchr(start, '\n', (size_t)(end - start))) != NULL) {
        *nl = '\0';
        run_command(idx, start);
        if (fds[NUM_POLL_FDS + idx].fd < 0)      
            return;
        start = nl + 1;
    }
    ctrl_len[idx] = (size_t)(end - start);
    memmove(ctrl_buf[idx], start, ctrl_len[idx]);

    if (ctrl_len[idx] >= sizeof(ctrl_buf[idx]) - 1) {   
        ctrl_len[idx] = 0;
        if (send_all(fd, "ERROR: Command too long\n", 24) < 0)
            close_ctrl(idx);
    }
}

static void check_file(void)
{
    struct stat st;
    if (stat(FILE_PATH, &st) < 0)
        return;                         
    if (st.st_size != g_last_size) {
        g_last_size = st.st_size;
        if (g_active) {
            printf("[FILE_EVENT] %s size changed to %lld bytes\n",
                   FILE_PATH, (long long)st.st_size);
            fflush(stdout);
            g_events++;
        }
    }
}


int main(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_signal;          
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGTERM, &sa, NULL) < 0 || sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    signal(SIGPIPE, SIG_IGN);

    for (int i = 0; i < TOTAL_FDS; i++) {
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }

    int fifo_fd = setup_fifo();
    if (fifo_fd < 0)
        return EXIT_FAILURE;
    int sock_fd = setup_socket();
    if (sock_fd < 0) {
        close(fifo_fd);
        return EXIT_FAILURE;
    }
    int file_fd = setup_file();
    if (file_fd < 0) {
        close(fifo_fd);
        close(sock_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    fds[FD_FIFO].fd = fifo_fd;            
    fds[FD_FIFO].events = POLLIN;
    fds[FD_SOCKET_LISTENER].fd = sock_fd; 
    fds[FD_SOCKET_LISTENER].events = POLLIN;
    fds[FD_FILE].fd = file_fd;            
    fds[FD_FILE].events = POLLERR;

    g_start = time(NULL);
    printf("[Monitor] Listening on %s\n", SOCKET_PATH);
    printf("[Monitor] Monitoring %s (FIFO) and %s\n", FIFO_PATH, FILE_PATH);
    fflush(stdout);

    while (!g_stop) {
        int ready = poll(fds, TOTAL_FDS, POLL_TIMEOUT_MS);
        if (ready < 0) {
            if (errno == EINTR)
                continue;
            perror("poll");
            break;
        }

        if (ready > 0) {
            if (fds[FD_FIFO].revents & POLLIN)
                handle_fifo(fds[FD_FIFO].fd);
            if (fds[FD_SOCKET_LISTENER].revents & POLLIN)
                accept_ctrl(fds[FD_SOCKET_LISTENER].fd);
            for (int i = 0; i < MAX_CTRL_CLIENTS; i++) {
                struct pollfd *p = &fds[NUM_POLL_FDS + i];
                if (p->fd >= 0 && (p->revents & (POLLIN | POLLHUP | POLLERR)))
                    handle_ctrl(i);
            }
        }

        check_file();

        if (ready == 0) {               
            printf("[HEARTBEAT] Monitor alive, events_seen=%lu%s\n",
                   g_events, g_active ? "" : " (paused)");
            fflush(stdout);
        }
    }

    for (int i = 0; i < MAX_CTRL_CLIENTS; i++)
        if (fds[NUM_POLL_FDS + i].fd >= 0)
            close(fds[NUM_POLL_FDS + i].fd);
    close(fifo_fd);
    close(sock_fd);
    close(file_fd);
    unlink(SOCKET_PATH);
    printf("\n[Monitor] Shutdown complete.\n");
    return EXIT_SUCCESS;
}