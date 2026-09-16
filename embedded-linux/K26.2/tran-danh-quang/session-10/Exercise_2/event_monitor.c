#define _POSIX_C_SOURCE 200809L

#include "event_monitor.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

/* Global loop flag updated exclusively by signal handlers */
static volatile sig_atomic_t g_monitor_running = 1;

/* Signal handler for SIGTERM and SIGINT */
static void sigterm_handler(int signum) {
    (void)signum;
    g_monitor_running = 0;
}

/* Register signals cleanly */
static int setup_signals(void) {
    struct sigaction sa;
    (void)memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigterm_handler;
    (void)sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM)");
        return STATUS_ERROR;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction(SIGINT)");
        return STATUS_ERROR;
    }
    return STATUS_SUCCESS;
}

/* Initialize FIFO: create if missing, open with O_RDWR|O_NONBLOCK to prevent infinite EOF loops */
static int setup_fifo(const char *fifo_path, int *out_fd) {
    if (fifo_path == NULL || out_fd == NULL) {
        return STATUS_ERROR;
    }

    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return STATUS_ERROR;
        }
    }

    int fd = open(fifo_path, O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        perror("open(fifo)");
        return STATUS_ERROR;
    }

    *out_fd = fd;
    return STATUS_SUCCESS;
}

/* Initialize Unix Domain listening socket */
static int setup_unix_socket(const char *sock_path, int *out_fd) {
    if (sock_path == NULL || out_fd == NULL) {
        return STATUS_ERROR;
    }

    /* Unlink existing socket file if present */
    if (unlink(sock_path) == -1 && errno != ENOENT) {
        perror("unlink(sock_path)");
        return STATUS_ERROR;
    }

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket(AF_UNIX)");
        return STATUS_ERROR;
    }

    struct sockaddr_un addr;
    (void)memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    (void)strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind(AF_UNIX)");
        (void)close(fd);
        return STATUS_ERROR;
    }

    if (listen(fd, 5) == -1) {
        perror("listen(AF_UNIX)");
        (void)close(fd);
        return STATUS_ERROR;
    }

    *out_fd = fd;
    return STATUS_SUCCESS;
}

/* Initialize regular file for tracking */
static int setup_file(const char *file_path, int *out_fd, off_t *out_initial_size) {
    if (file_path == NULL || out_fd == NULL || out_initial_size == NULL) {
        return STATUS_ERROR;
    }

    /* Create file if it does not exist */
    int fd = open(file_path, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("open(file_path)");
        return STATUS_ERROR;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat(file_path)");
        (void)close(fd);
        return STATUS_ERROR;
    }

    *out_fd = fd;
    *out_initial_size = st.st_size;
    return STATUS_SUCCESS;
}

/* Read lines from FIFO and print event */
static int handle_fifo_event(int fifo_fd, int *events_seen) {
    if (fifo_fd < 0 || events_seen == NULL) {
        return STATUS_ERROR;
    }

    char buf[BUFFER_SIZE];
    (void)memset(buf, 0, sizeof(buf));

    ssize_t bytes = read(fifo_fd, buf, sizeof(buf) - 1);
    if (bytes > 0) {
        buf[bytes] = '\0';
        char *line = strtok(buf, "\r\n");
        while (line != NULL) {
            while (*line == ' ') {
                line++;
            }
            if (*line != '\0') {
                (void)printf("[FIFO_EVENT] %s\n", line);
                (*events_seen)++;
            }
            line = strtok(NULL, "\r\n");
        }
    }
    return STATUS_SUCCESS;
}

/* Handle control command from client connected to Unix socket */
static int handle_socket_event(int listener_fd, int *events_seen, time_t start_time, int *active_flag) {
    if (listener_fd < 0 || events_seen == NULL || active_flag == NULL) {
        return STATUS_ERROR;
    }

    struct sockaddr_un client_addr;
    socklen_t addr_len = sizeof(client_addr);
    (void)memset(&client_addr, 0, sizeof(client_addr));

    int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd == -1) {
        if (errno == EINTR) {
            return STATUS_SUCCESS;
        }
        perror("accept(AF_UNIX)");
        return STATUS_ERROR;
    }

    char recv_buf[BUFFER_SIZE];
    (void)memset(recv_buf, 0, sizeof(recv_buf));

    ssize_t bytes = recv(client_fd, recv_buf, sizeof(recv_buf) - 1, 0);
    if (bytes > 0) {
        recv_buf[bytes] = '\0';
        /* Trim newlines and spaces */
        char *cmd = strtok(recv_buf, "\r\n");
        if (cmd != NULL) {
            while (*cmd == ' ') {
                cmd++;
            }

            char resp_buf[BUFFER_SIZE];
            (void)memset(resp_buf, 0, sizeof(resp_buf));

            if (strncmp(cmd, "STATUS", 6) == 0) {
                time_t now = time(NULL);
                long uptime = (now != (time_t)-1) ? (long)(now - start_time) : 0L;
                int len = snprintf(resp_buf, sizeof(resp_buf),
                                   "Total events: %d, Uptime: %ld seconds\n",
                                   *events_seen, uptime);
                if (len > 0) {
                    (void)send(client_fd, resp_buf, (size_t)len, 0);
                }
            } else if (strncmp(cmd, "START", 5) == 0) {
                *active_flag = 1;
                int len = snprintf(resp_buf, sizeof(resp_buf), "OK\n");
                if (len > 0) {
                    (void)send(client_fd, resp_buf, (size_t)len, 0);
                }
            } else if (strncmp(cmd, "STOP", 4) == 0) {
                *active_flag = 0;
                int len = snprintf(resp_buf, sizeof(resp_buf), "OK\n");
                if (len > 0) {
                    (void)send(client_fd, resp_buf, (size_t)len, 0);
                }
            } else if (strncmp(cmd, "EXIT", 4) == 0) {
                /* Close client connection cleanly */
            } else {
                int len = snprintf(resp_buf, sizeof(resp_buf), "ERROR: Unknown command\n");
                if (len > 0) {
                    (void)send(client_fd, resp_buf, (size_t)len, 0);
                }
            }
        }
    }

    if (close(client_fd) == -1) {
        perror("close(client_fd)");
    }
    return STATUS_SUCCESS;
}

/* Check if the tracked file size has changed */
static int check_file_size_change(const char *file_path, off_t *last_size, int *events_seen) {
    if (file_path == NULL || last_size == NULL || events_seen == NULL) {
        return STATUS_ERROR;
    }

    struct stat st;
    if (stat(file_path, &st) == -1) {
        /* File may have been deleted or temporarily inaccessible */
        return STATUS_ERROR;
    }

    if (st.st_size != *last_size) {
        (void)printf("[FILE_EVENT] %s size changed to %ld bytes\n", file_path, (long)st.st_size);
        *last_size = st.st_size;
        (*events_seen)++;
    }
    return STATUS_SUCCESS;
}

/* Close all open descriptors and remove socket file */
static int cleanup_monitor(int fifo_fd, int sock_fd, int file_fd, const char *sock_path) {
    if (fifo_fd >= 0) {
        if (close(fifo_fd) == -1) {
            perror("close(fifo_fd)");
        }
    }
    if (sock_fd >= 0) {
        if (close(sock_fd) == -1) {
            perror("close(sock_fd)");
        }
    }
    if (file_fd >= 0) {
        if (close(file_fd) == -1) {
            perror("close(file_fd)");
        }
    }
    if (sock_path != NULL) {
        if (unlink(sock_path) == -1 && errno != ENOENT) {
            perror("unlink(sock_path)");
        }
    }
    (void)printf("[Monitor] Shutdown complete.\n");
    return STATUS_SUCCESS;
}

int main(void) {
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0) {
        perror("setvbuf(stdout)");
        return EXIT_FAILURE;
    }

    if (setup_signals() != STATUS_SUCCESS) {
        return EXIT_FAILURE;
    }

    int fifo_fd = -1;
    if (setup_fifo(FIFO_PATH, &fifo_fd) != STATUS_SUCCESS) {
        return EXIT_FAILURE;
    }

    int sock_fd = -1;
    if (setup_unix_socket(SOCKET_PATH, &sock_fd) != STATUS_SUCCESS) {
        (void)close(fifo_fd);
        return EXIT_FAILURE;
    }

    int file_fd = -1;
    off_t last_file_size = 0;
    if (setup_file(FILE_PATH, &file_fd, &last_file_size) != STATUS_SUCCESS) {
        (void)close(fifo_fd);
        (void)close(sock_fd);
        (void)unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    (void)printf("[Monitor] Listening on %s\n", SOCKET_PATH);
    (void)printf("[Monitor] Monitoring %s (FIFO) and %s\n", FIFO_PATH, FILE_PATH);

    struct pollfd fds[NUM_POLL_FDS];
    fds[FD_FIFO].fd = fifo_fd;
    fds[FD_FIFO].events = POLLIN;
    fds[FD_FIFO].revents = 0;

    fds[FD_SOCKET_LISTENER].fd = sock_fd;
    fds[FD_SOCKET_LISTENER].events = POLLIN;
    fds[FD_SOCKET_LISTENER].revents = 0;

    /* Regular files cannot be polled for write readiness in Linux; monitored via stat */
    fds[FD_FILE].fd = file_fd;
    fds[FD_FILE].events = 0;
    fds[FD_FILE].revents = 0;

    int events_seen = 0;
    int is_active = 1;
    time_t start_time = time(NULL);

    while (g_monitor_running) {
        fds[FD_FIFO].revents = 0;
        fds[FD_SOCKET_LISTENER].revents = 0;
        fds[FD_FILE].revents = 0;

        int ready = poll(fds, NUM_POLL_FDS, POLL_TIMEOUT_MS);
        if (ready < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            break;
        }

        if (ready == 0) {
            /* 2-second timeout expired */
            (void)printf("[HEARTBEAT] Monitor alive, events_seen=%d\n", events_seen);
        } else {
            /* Check FIFO events */
            if (fds[FD_FIFO].revents & POLLIN) {
                if (is_active) {
                    (void)handle_fifo_event(fifo_fd, &events_seen);
                } else {
                    /* Read and discard if monitoring is stopped */
                    char discard[BUFFER_SIZE];
                    (void)read(fifo_fd, discard, sizeof(discard));
                }
            }

            /* Check Unix socket control commands */
            if (fds[FD_SOCKET_LISTENER].revents & POLLIN) {
                (void)handle_socket_event(sock_fd, &events_seen, start_time, &is_active);
            }
        }

        /* Check if file size modified */
        if (is_active) {
            (void)check_file_size_change(FILE_PATH, &last_file_size, &events_seen);
        }
    }

    (void)cleanup_monitor(fifo_fd, sock_fd, file_fd, SOCKET_PATH);
    return EXIT_SUCCESS;
}
