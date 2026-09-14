#include "../common.h"

static pthread_mutex_t g_server_output_lock = PTHREAD_MUTEX_INITIALIZER;

void render_server_dashboard(const struct AgentEntry *agents, size_t count) {
    pthread_mutex_lock(&g_server_output_lock);
    printf("\033[2J\033[H");
    printf("=== Infra Health Monitor ===\n");
    for (size_t i = 0; i < count; ++i) {
        const struct AgentEntry *entry = &agents[i];
         char cpu_bar[128];
         char ram_bar[128];
         char disk_bar[128];
         render_bar(entry->last_data.cpu, "cpu", &entry->config, entry->status, cpu_bar, sizeof(cpu_bar));
         render_bar(entry->last_data.ram, "ram", &entry->config, entry->status, ram_bar, sizeof(ram_bar));
         render_bar(entry->last_data.disk, "disk", &entry->config, entry->status, disk_bar, sizeof(disk_bar));
        printf("[%s] %s\n", entry->agent_id[0] ? entry->agent_id : "unknown",
             entry->status == STATUS_ONLINE ? "ONLINE" : "--- OFFLINE ---");
         printf("  CPU  %s %.1f%% %s\n", cpu_bar, entry->last_data.cpu,
             metric_status(entry->last_data.cpu, "cpu", &entry->config, entry->status));
         printf("  RAM  %s %.1f%% %s\n", ram_bar, entry->last_data.ram,
             metric_status(entry->last_data.ram, "ram", &entry->config, entry->status));
         printf("  DISK %s %.1f%% %s\n", disk_bar, entry->last_data.disk,
             metric_status(entry->last_data.disk, "disk", &entry->config, entry->status));
    }
    printf("> /_\n");
    fflush(stdout);
    pthread_mutex_unlock(&g_server_output_lock);
}
