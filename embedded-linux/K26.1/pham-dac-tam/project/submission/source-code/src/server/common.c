#include "../common.h"

void append_json_log(const char *path, const char *line) {
    static int log_dir_ready = 0;
    if (!log_dir_ready) {
        if (mkdir(LOG_DIR, 0755) < 0 && errno != EEXIST) {
            perror("mkdir logs");
        }
        log_dir_ready = 1;
    }
    FILE *fp = fopen(path, "a");
    if (!fp) return;
    fprintf(fp, "%s\n", line);
    fclose(fp);
}

const char *metric_status(double percent, const char *metric_type, const struct Config *config, int status) {
    double warning = 70.0;
    double critical = 90.0;
    if (config && metric_type) {
        if (strcmp(metric_type, "ram") == 0) {
            warning = config->ram_warning;
            critical = config->ram_critical;
        } else if (strcmp(metric_type, "disk") == 0) {
            warning = config->disk_warning;
            critical = config->disk_critical;
        } else {
            warning = config->cpu_warning;
            critical = config->cpu_critical;
        }
    }
    if (status == STATUS_OFFLINE) {
        return "OFFLINE";
    }
    if (percent >= critical) return "CRITICAL";
    if (percent >= warning) return "WARNING";
    return "NORMAL";
}

void render_bar(double percent, const char *metric_type, const struct Config *config, int status, char *out, size_t out_size) {
    const char *color = "\033[32m";
    int filled;
    size_t used;

    if (!out || out_size == 0) return;
    const char *state = metric_status(percent, metric_type, config, status);
    if (strcmp(state, "WARNING") == 0) color = "\033[33m";
    if (strcmp(state, "CRITICAL") == 0 || strcmp(state, "OFFLINE") == 0) color = "\033[31m";
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    filled = (int)((percent / 100.0) * BAR_WIDTH);
    if (filled > BAR_WIDTH) filled = BAR_WIDTH;
    int prefix_written = snprintf(out, out_size, "%s[", color);
    if (prefix_written < 0) {
        /* real encoding failure, not just truncation - fall back to a safe placeholder */
        snprintf(out, out_size, "[N/A]");
        return;
    }
    if ((size_t)prefix_written >= out_size) {
        out[out_size - 1] = '\0';
        return;
    }
    used = strlen(out);
    for (int i = 0; i < BAR_WIDTH; ++i) {
        const char *glyph = (i < filled) ? "\xE2\x96\x88" : "\xE2\x96\x91";
        int written = snprintf(out + used, out_size - used, "%s", glyph);
        if (written < 0) {
            out[used] = '\0';
            return;
        }
        if ((size_t)written >= out_size - used) {
            out[used] = '\0';
            return;
        }
        used += (size_t)written;
    }
    snprintf(out + used, out_size - used, "]\033[0m");
}

void log_periodic_data(const char *agent_id, const struct Metrics *metrics) {
    char path[256];
    snprintf(path, sizeof(path), "%s/periodic.log", LOG_DIR);
    char line[512];
    snprintf(line, sizeof(line), "{\"ts\":%ld,\"agent_id\":\"%s\",\"cpu\":%.1f,\"ram\":%.1f,\"disk\":%.1f}", time(NULL), agent_id, metrics->cpu, metrics->ram, metrics->disk);
    append_json_log(path, line);
}

void log_alert(const char *agent_id, const char *metric, double value, const char *threshold) {
    char path[256];
    snprintf(path, sizeof(path), "%s/alert.log", LOG_DIR);
    char line[512];
    snprintf(line, sizeof(line), "{\"ts\":%ld,\"agent_id\":\"%s\",\"metric\":\"%s\",\"value\":%.1f,\"threshold\":\"%s\"}", time(NULL), agent_id, metric, value, threshold);
    append_json_log(path, line);
}

void log_event(const char *agent_id, const char *event, const char *details) {
    char path[256];
    snprintf(path, sizeof(path), "%s/events.log", LOG_DIR);
    char line[512];
    snprintf(line, sizeof(line), "{\"ts\":%ld,\"agent_id\":\"%s\",\"event\":\"%s\",\"details\":\"%s\"}", time(NULL), agent_id, event, details);
    append_json_log(path, line);
}

struct AgentEntry *find_agent_entry(struct AgentEntry *agents, size_t count, const char *agent_id) {
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(agents[i].agent_id, agent_id) == 0) return &agents[i];
    }
    return NULL;
}

int send_message(int fd, const char *msg) {
    return send(fd, msg, strlen(msg), 0) >= 0;
}

static int extract_string_field(const char *line, const char *field, char *out, size_t out_size) {
    char pattern[128];
    int pattern_len = snprintf(pattern, sizeof(pattern), "\"%s\"", field);
    if (pattern_len < 0 || (size_t)pattern_len >= sizeof(pattern)) return 0;
    const char *p = strstr(line, pattern);
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    if (*p != '"') return 0;
    p++;
    const char *q = strchr(p, '"');
    if (!q) return 0;
    size_t len = (size_t)(q - p);
    if (len >= out_size) len = out_size - 1;
    memcpy(out, p, len);
    out[len] = '\0';
    return (out[0] != '\0');
}

static int extract_double_field(const char *line, const char *field, double *out) {
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", field);
    const char *p = strstr(line, pattern);
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    *out = strtod(p, NULL);
    return 1;
}

int parse_message(const char *line, struct Message *msg) {
    memset(msg, 0, sizeof(*msg));
    if (!extract_string_field(line, "type", msg->type, sizeof(msg->type))) {
        return 0;
    }
    if (!extract_string_field(line, "agent_id", msg->agent_id, sizeof(msg->agent_id))) {
        return 0;
    }
    if (strcmp(msg->type, "config") == 0) {
        if (!extract_string_field(line, "key", msg->key, sizeof(msg->key))) return 0;
        if (!extract_string_field(line, "value", msg->value, sizeof(msg->value))) return 0;
        return 1;
    }
    if (strcmp(msg->type, "data") == 0) {
        if (extract_double_field(line, "cpu", &msg->cpu) &&
            extract_double_field(line, "ram", &msg->ram) &&
            extract_double_field(line, "disk", &msg->disk)) {
            msg->has_metrics = 1;
            return 1;
        }
        return 0;
    }
    if (strcmp(msg->type, "heartbeat") == 0) {
        return 1;
    }
    if (strcmp(msg->type, "bye") == 0) {
        return 1;
    }
    return 0;
}

int format_data_message(char *buf, size_t size, const char *agent_id, const struct Metrics *metrics) {
    return snprintf(buf, size, "{\"type\":\"data\",\"agent_id\":\"%s\",\"cpu\":%.1f,\"ram\":%.1f,\"disk\":%.1f}\n", agent_id, metrics->cpu, metrics->ram, metrics->disk) > 0;
}

int format_heartbeat_message(char *buf, size_t size, const char *agent_id) {
    return snprintf(buf, size, "{\"type\":\"heartbeat\",\"agent_id\":\"%s\"}\n", agent_id) > 0;
}

int format_bye_message(char *buf, size_t size, const char *agent_id) {
    return snprintf(buf, size, "{\"type\":\"bye\",\"agent_id\":\"%s\"}\n", agent_id) > 0;
}

int format_config_message(char *buf, size_t size, const char *agent_id, const char *key, const char *value) {
    return snprintf(buf, size, "{\"type\":\"config\",\"agent_id\":\"%s\",\"key\":\"%s\",\"value\":\"%s\"}\n", agent_id, key, value) > 0;
}

int format_ack_message(char *buf, size_t size, const char *status) {
    return snprintf(buf, size, "{\"type\":\"ack\",\"status\":\"%s\"}\n", status) > 0;
}

void update_entry_from_message(struct AgentEntry *entry, const struct Message *msg) {
    if (!entry || !msg) return;
    entry->last_heartbeat_time = time(NULL);
    entry->last_seen = time(NULL);
    if (msg->has_metrics) {
        entry->last_data.cpu = msg->cpu;
        entry->last_data.ram = msg->ram;
        entry->last_data.disk = msg->disk;
    }
}

void mark_offline_entries(struct AgentEntry *agents, size_t count) {
    time_t now = time(NULL);
    for (size_t i = 0; i < count; ++i) {
        if (agents[i].status == STATUS_ONLINE && now - agents[i].last_heartbeat_time > AGENT_TIMEOUT_SEC) {
            agents[i].status = STATUS_OFFLINE;
            log_event(agents[i].agent_id, "offline", "timeout");
        }
    }
}

