#include "../common.h"

static long extract_ts_from_log_line(const char *line) {
    const char *p = strstr(line, "\"ts\"");
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    p++;
    while (*p == ' ' || *p == '\t') p++;
    return strtol(p, NULL, 10);
}

int apply_config_to_agent(struct AgentEntry *agents, size_t count, const char *agent_id, const char *key, const char *value) {
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(agents[i].agent_id, agent_id) == 0) {
            if (strcmp(key, "interval") == 0) {
                int interval = atoi(value);
                if (interval <= 0) interval = 3;
                agents[i].interval = interval;
                char msgbuf[MAX_LINE];
                if (format_config_message(msgbuf, sizeof(msgbuf), agents[i].agent_id, key, value)) {
                    send_message(agents[i].fd, msgbuf);
                }
            }
            return 1;
        }
    }
    return 0;
}

void print_history(const char *agent_id, int last_n) {
    struct Entry {
        long ts;
        char line[512];
    } entries[256];
    int count = 0;
    const char *paths[] = {"logs/periodic.log", "logs/alert.log"};

    for (size_t p = 0; p < sizeof(paths) / sizeof(paths[0]); ++p) {
        FILE *fp = fopen(paths[p], "r");
        if (!fp) continue;
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, agent_id) == NULL) continue;
            if (count < (int)(sizeof(entries) / sizeof(entries[0]))) {
                entries[count].ts = extract_ts_from_log_line(line);
                snprintf(entries[count].line, sizeof(entries[count].line), "%s", line);
                count++;
            }
        }
        fclose(fp);
    }

    if (count <= 1) {
        for (int i = 0; i < count; ++i) printf("%s", entries[i].line);
        return;
    }

    for (int i = 0; i < count; ++i) {
        for (int j = i + 1; j < count; ++j) {
            if (entries[j].ts < entries[i].ts) {
                struct Entry tmp = entries[i];
                entries[i] = entries[j];
                entries[j] = tmp;
            }
        }
    }

    int limit = (last_n > 0 && last_n < count) ? last_n : count;
    for (int i = count - limit; i < count; ++i) {
        printf("%s", entries[i].line);
    }
}

int handle_command(const char *line, struct AgentEntry *agents, size_t *count, size_t *capacity) {
    (void)capacity;
    if (strncmp(line, "/help", 5) == 0) {
        printf("/help\n/config <agent_id> <key>=<value>\n/history <agent_id> [--last N]\n");
        return 1;
    }
    if (strncmp(line, "/config", 7) == 0) {
        char agent_id[64], key[32], value[32];
        if (sscanf(line, "/config %63s %31[^=]=%31s", agent_id, key, value) == 3) {
            int ok = apply_config_to_agent(agents, *count, agent_id, key, value);
            printf(ok ? "[OK] applied config to %s\n" : "[ERR] agent_id not found\n", agent_id);
            return 1;
        }
    }
    if (strncmp(line, "/history", 8) == 0) {
        char agent_id[64];
        int last_n = 10;
        if (sscanf(line, "/history %63s --last %d", agent_id, &last_n) == 2 ||
            sscanf(line, "/history %63s %d", agent_id, &last_n) == 2 ||
            sscanf(line, "/history %63s", agent_id) == 1) {
            print_history(agent_id, last_n > 0 ? last_n : 10);
            return 1;
        }
    }
    return 0;
}
