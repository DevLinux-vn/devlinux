#include "../common.h"

void append_json_log(const char *path, const char *line) {
    mkdir(LOG_DIR, 0755);
    FILE *fp = fopen(path, "a");
    if (!fp) return;
    fprintf(fp, "%s\n", line);
    fclose(fp);
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
    char pattern[64];
    snprintf(pattern, sizeof(pattern), "\"%s\"", field);
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
    return 0;
}

int format_data_message(char *buf, size_t size, const char *agent_id, const struct Metrics *metrics) {
    return snprintf(buf, size, "{\"type\":\"data\",\"agent_id\":\"%s\",\"cpu\":%.1f,\"ram\":%.1f,\"disk\":%.1f}\n", agent_id, metrics->cpu, metrics->ram, metrics->disk) > 0;
}

int format_heartbeat_message(char *buf, size_t size, const char *agent_id) {
    return snprintf(buf, size, "{\"type\":\"heartbeat\",\"agent_id\":\"%s\"}\n", agent_id) > 0;
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

