#include "../common.h"

static int create_listener(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    listen(sock, 10);
    return sock;
}

static void ensure_capacity(struct AgentEntry **agents, size_t *capacity, size_t needed) {
    if (needed < *capacity) return;
    size_t new_cap = (*capacity == 0) ? 8 : *capacity;
    while (new_cap < needed) new_cap *= 2;
    struct AgentEntry *new_agents = realloc(*agents, new_cap * sizeof(**agents));
    if (!new_agents) {
        perror("realloc");
        exit(1);
    }
    memset(new_agents + *capacity, 0, (new_cap - *capacity) * sizeof(**agents));
    *agents = new_agents;
    *capacity = new_cap;
}

static void remove_agent_entry(struct AgentEntry **agents, size_t *count, size_t idx) {
    if (!agents || !*agents || idx >= *count) return;
    for (size_t i = idx + 1; i < *count; ++i) {
        (*agents)[i - 1] = (*agents)[i];
    }
    (*count)--;
}

int main(int argc, char **argv) {
    int port = (argc > 1) ? atoi(argv[1]) : DEFAULT_PORT;
    int listen_fd = create_listener(port);
    if (listen_fd < 0) {
        perror("listen");
        return 1;
    }

    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    struct itimerspec its;
    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = 2;
    its.it_interval.tv_sec = 2;
    timerfd_settime(timer_fd, 0, &its, NULL);

    int epfd = epoll_create1(0);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    ev.events = EPOLLIN;
    ev.data.fd = timer_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, timer_fd, &ev);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    struct AgentEntry *agents = NULL;
    size_t count = 0;
    size_t capacity = 0;

    while (1) {
        struct epoll_event events[MAX_EVENTS];
        int n = epoll_wait(epfd, events, MAX_EVENTS, 1000);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {
                int client_fd = accept(listen_fd, NULL, NULL);
                if (client_fd >= 0) {
                    enable_keepalive(client_fd, 5, 2, 3);
                    ensure_capacity(&agents, &capacity, count + 1);
                    struct AgentEntry *entry = &agents[count++];
                    memset(entry, 0, sizeof(*entry));
                    entry->fd = client_fd;
                    entry->status = STATUS_ONLINE;
                    entry->interval = 3;
                    entry->last_heartbeat_time = time(NULL);
                    entry->last_seen = time(NULL);
                    entry->inbuf_len = 0;
                    ev.events = EPOLLIN;
                    ev.data.fd = client_fd;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                    log_event("new", "connect", "accepted");
                }
            } else if (fd == STDIN_FILENO) {
                char input[MAX_LINE];
                ssize_t nb = read(STDIN_FILENO, input, sizeof(input) - 1);
                if (nb > 0) {
                    input[nb] = '\0';
                    char *line = strtok(input, "\n");
                    while (line) {
                        handle_command(line, agents, &count, &capacity);
                        line = strtok(NULL, "\n");
                    }
                }
            } else if (fd == timer_fd) {
                uint64_t expirations = 0;
                read(timer_fd, &expirations, sizeof(expirations));
                render_server_dashboard(agents, count);
            } else {
                char buffer[MAX_LINE];
                ssize_t nb = recv(fd, buffer, sizeof(buffer) - 1, 0);
                if (nb <= 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    for (size_t j = 0; j < count; ++j) {
                        if (agents[j].fd == fd) {
                            log_event(agents[j].agent_id, "disconnect", "socket closed");
                            remove_agent_entry(&agents, &count, j);
                            break;
                        }
                    }
                } else {
                    buffer[nb] = '\0';
                    for (size_t j = 0; j < count; ++j) {
                        if (agents[j].fd == fd) {
                            struct AgentEntry *entry = &agents[j];
                            size_t copied = 0;
                            while (copied < (size_t)nb) {
                                size_t room = sizeof(entry->inbuf) - entry->inbuf_len - 1;
                                if (room == 0) {
                                    entry->inbuf[0] = '\0';
                                    entry->inbuf_len = 0;
                                    room = sizeof(entry->inbuf) - 1;
                                }
                                size_t take = room < (size_t)(nb - copied) ? room : (size_t)(nb - copied);
                                memcpy(entry->inbuf + entry->inbuf_len, buffer + copied, take);
                                entry->inbuf_len += take;
                                copied += take;
                                entry->inbuf[entry->inbuf_len] = '\0';

                                char *line = NULL;
                                while ((line = strchr(entry->inbuf, '\n')) != NULL) {
                                    *line = '\0';
                                    if (line > entry->inbuf && *(line - 1) == '\r') *(line - 1) = '\0';
                                    if (entry->inbuf[0] != '\0') {
                                        struct Message msg;
                                        if (parse_message(entry->inbuf, &msg)) {
                                            strncpy(entry->agent_id, msg.agent_id, sizeof(entry->agent_id) - 1);
                                            update_entry_from_message(entry, &msg);
                                            entry->status = STATUS_ONLINE;
                                            if (strcmp(msg.type, "data") == 0) {
                                                entry->last_data.cpu = msg.cpu;
                                                entry->last_data.ram = msg.ram;
                                                entry->last_data.disk = msg.disk;
                                                log_periodic_data(entry->agent_id, &entry->last_data);
                                                if (entry->last_data.cpu >= 90.0) log_alert(entry->agent_id, "CPU", entry->last_data.cpu, "critical");
                                                if (entry->last_data.ram >= 90.0) log_alert(entry->agent_id, "RAM", entry->last_data.ram, "critical");
                                                if (entry->last_data.disk >= 95.0) log_alert(entry->agent_id, "DISK", entry->last_data.disk, "critical");
                                            } else if (strcmp(msg.type, "heartbeat") == 0) {
                                                log_event(entry->agent_id, "heartbeat", "ok");
                                            }
                                        }
                                    }
                                    size_t remaining = strlen(line + 1);
                                    memmove(entry->inbuf, line + 1, remaining + 1);
                                    entry->inbuf_len = remaining;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }
        mark_offline_entries(agents, count);
    }

    free(agents);
    close(timer_fd);
    close(listen_fd);
    return 0;
}
