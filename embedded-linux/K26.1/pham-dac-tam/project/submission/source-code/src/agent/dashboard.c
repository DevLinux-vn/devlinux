#include "../common.h"

static pthread_mutex_t g_output_lock = PTHREAD_MUTEX_INITIALIZER;

void render_agent_dashboard(const char *agent_id, const struct Metrics *metrics, const struct Config *config, int connected, int status) {
    char cpu_bar[128];
    char ram_bar[128];
    char disk_bar[128];
    render_bar(metrics->cpu, "cpu", config, status, cpu_bar, sizeof(cpu_bar));
    render_bar(metrics->ram, "ram", config, status, ram_bar, sizeof(ram_bar));
    render_bar(metrics->disk, "disk", config, status, disk_bar, sizeof(disk_bar));

    pthread_mutex_lock(&g_output_lock);
    printf("\033[2J\033[H");
    printf("=== DevLinux Health Agent ===\n");
    printf("Agent ID: %s\n", agent_id);
    printf("Status: %s\n", connected ? "CONNECTED" : "DISCONNECTED");
    printf("CPU   %s %.1f%% %s\n", cpu_bar, metrics->cpu, metric_status(metrics->cpu, "cpu", config, status));
    printf("RAM   %s %.1f%% %s\n", ram_bar, metrics->ram, metric_status(metrics->ram, "ram", config, status));
    printf("DISK  %s %.1f%% %s\n", disk_bar, metrics->disk, metric_status(metrics->disk, "disk", config, status));
    fflush(stdout);
    pthread_mutex_unlock(&g_output_lock);
}
