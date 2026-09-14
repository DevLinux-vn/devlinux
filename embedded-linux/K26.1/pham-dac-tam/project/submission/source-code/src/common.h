#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_PORT 9000
#define DEFAULT_HOST "127.0.0.1"
#define MAX_EVENTS 64
#define MAX_LINE 4096
#define LOG_DIR "logs"
#define AGENT_ID_FILE "agent.id"
#define AGENT_TIMEOUT_SEC 9
#define REFRESH_SEC 2
#define HEARTBEAT_SEC 3
#define BAR_WIDTH 20

enum { STATUS_ONLINE = 1, STATUS_OFFLINE = 0 };

struct Metrics {
    double cpu;
    double ram;
    double disk;
    unsigned long rx_bytes;
    unsigned long tx_bytes;
};

struct Config {
    int interval;
    double cpu_warning;
    double cpu_critical;
    double ram_warning;
    double ram_critical;
    double disk_warning;
    double disk_critical;
};

struct AgentEntry {
    int fd;
    int status;
    int interval;
    char agent_id[64];
    struct Config config;
    struct Metrics last_data;
    time_t last_heartbeat_time;
    time_t last_seen;
    char inbuf[MAX_LINE];
    size_t inbuf_len;
};

struct Message {
    char type[16];
    char agent_id[64];
    char key[32];
    char value[32];
    double cpu;
    double ram;
    double disk;
    int has_metrics;
};

int load_or_create_agent_id(const char *path, char *out, size_t out_size);
void collect_metrics(struct Metrics *metrics);
void render_bar(double percent, const char *metric_type, const struct Config *config, int status, char *out, size_t out_size);
const char *metric_status(double percent, const char *metric_type, const struct Config *config, int status);
void render_agent_dashboard(const char *agent_id, const struct Metrics *metrics, const struct Config *config, int connected, int status);
void render_server_dashboard(const struct AgentEntry *agents, size_t count);
int parse_message(const char *line, struct Message *msg);
int format_data_message(char *buf, size_t size, const char *agent_id, const struct Metrics *metrics);
int format_heartbeat_message(char *buf, size_t size, const char *agent_id);
int format_config_message(char *buf, size_t size, const char *agent_id, const char *key, const char *value);
int format_ack_message(char *buf, size_t size, const char *status);
void append_json_log(const char *path, const char *line);
void log_periodic_data(const char *agent_id, const struct Metrics *metrics);
void log_alert(const char *agent_id, const char *metric, double value, const char *threshold);
void log_event(const char *agent_id, const char *event, const char *details);
void log_message(const char *agent_id, const char *event, const char *details, const char *log_name);
int enable_keepalive(int fd, int idle_sec, int interval_sec, int probes);
struct AgentEntry *find_agent_entry(struct AgentEntry *agents, size_t count, const char *agent_id);
int send_message(int fd, const char *msg);
int handle_command(const char *line, struct AgentEntry *agents, size_t *count, size_t *capacity);
void mark_offline_entries(struct AgentEntry *agents, size_t count);
void update_entry_from_message(struct AgentEntry *entry, const struct Message *msg);
void set_default_config(struct Config *cfg);
int apply_config_to_agent(struct AgentEntry *agents, size_t count, const char *agent_id, const char *key, const char *value);
void print_history(const char *agent_id, int last_n);

#endif
