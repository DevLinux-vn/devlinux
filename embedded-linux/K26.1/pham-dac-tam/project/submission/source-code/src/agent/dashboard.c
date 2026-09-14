#include "../common.h"

void render_bar(double percent, int width, char *out, size_t out_size) {
    (void)out_size;
    int filled = (int)((percent / 100.0) * width);
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    size_t i;
    for (i = 0; i < (size_t)filled; ++i) out[i] = '#';
    for (; i < (size_t)width; ++i) out[i] = '-';
    out[width] = '\0';
}

void render_agent_dashboard(const char *agent_id, const struct Metrics *metrics, int connected, int status) {
    char cpu_bar[BAR_WIDTH + 1];
    char ram_bar[BAR_WIDTH + 1];
    char disk_bar[BAR_WIDTH + 1];
    render_bar(metrics->cpu, BAR_WIDTH, cpu_bar, sizeof(cpu_bar));
    render_bar(metrics->ram, BAR_WIDTH, ram_bar, sizeof(ram_bar));
    render_bar(metrics->disk, BAR_WIDTH, disk_bar, sizeof(disk_bar));

    printf("\033[2J\033[H");
    printf("=== DevLinux Health Agent ===\n");
    printf("Agent ID: %s\n", agent_id);
    printf("Status: %s\n", connected ? "CONNECTED" : "DISCONNECTED");
    if (status == STATUS_OFFLINE) {
        printf("\033[31mOFFLINE\033[0m\n");
    }
    printf("CPU   [%s] %.1f%%\n", cpu_bar, metrics->cpu);
    printf("RAM   [%s] %.1f%%\n", ram_bar, metrics->ram);
    printf("DISK  [%s] %.1f%%\n", disk_bar, metrics->disk);
    fflush(stdout);
}
