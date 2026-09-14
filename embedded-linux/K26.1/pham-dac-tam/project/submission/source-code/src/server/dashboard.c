#include "../common.h"

static pthread_mutex_t g_server_output_lock = PTHREAD_MUTEX_INITIALIZER;

void render_server_dashboard(const struct AgentEntry *agents, size_t count) {
    pthread_mutex_lock(&g_server_output_lock);
    printf("\033[2J\033[H");
    printf("=== Infra Health Monitor ===\n");
    for (size_t i = 0; i < count; ++i) {
        const struct AgentEntry *entry = &agents[i];
        printf("[%s] %s\n", entry->agent_id[0] ? entry->agent_id : "unknown",
               entry->status == STATUS_ONLINE ? "ONLINE" : "OFFLINE");
        printf("  CPU  %.1f%%\n", entry->last_data.cpu);
        printf("  RAM  %.1f%%\n", entry->last_data.ram);
        printf("  DISK %.1f%%\n", entry->last_data.disk);
    }
    printf("> /_\n");
    fflush(stdout);
    pthread_mutex_unlock(&g_server_output_lock);
}
