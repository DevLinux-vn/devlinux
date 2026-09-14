#include "../common.h"

void render_server_dashboard(const struct AgentEntry *agents, size_t count) {
    printf("\033[2J\033[H");
    printf("=== Infra Health Monitor ===\n");
    for (size_t i = 0; i < count; ++i) {
        const struct AgentEntry *entry = &agents[i];
        printf("[%s] %s\n", entry->agent_id, entry->status == STATUS_ONLINE ? "ONLINE" : "OFFLINE");
        printf("  CPU  %.1f%%\n", entry->last_data.cpu);
        printf("  RAM  %.1f%%\n", entry->last_data.ram);
        printf("  DISK %.1f%%\n", entry->last_data.disk);
    }
    printf("> /_\n");
    fflush(stdout);
}
