#include "../common.h"

void log_message(const char *agent_id, const char *event, const char *details, const char *log_name) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s.log", LOG_DIR, log_name);
    char line[512];
    snprintf(line, sizeof(line), "{\"ts\":%ld,\"agent_id\":\"%s\",\"event\":\"%s\",\"details\":\"%s\"}",
             time(NULL), agent_id ? agent_id : "system", event, details ? details : "");
    append_json_log(path, line);
}
