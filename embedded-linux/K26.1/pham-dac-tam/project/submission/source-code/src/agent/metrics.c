#include "../common.h"

static void read_proc_stat(double *cpu_usage) {
    static unsigned long long prev_total = 0, prev_idle = 0;
    static int has_prev = 0;

    FILE *fp = fopen("/proc/stat", "r");
    if(!fp) { *cpu_usage = 0.0; return; }
    char line[256];
    int got_line = (fgets(line, sizeof(line), fp) != NULL);
    if (fclose(fp) != 0) {
        perror("fclose /proc/stat");
    }
    if (!got_line) {
        *cpu_usage = 0.0;
        return;
    }

    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    if (sscanf(line, "cpu  %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq) != 7) {
        *cpu_usage = 0.0;
        return;
    }
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
    unsigned long long idle_time = idle + iowait;

    if (!has_prev) {
        /* first sample only establishes the baseline; usage needs a second reading to form a delta */
        prev_total = total;
        prev_idle = idle_time;
        has_prev = 1;
        *cpu_usage = 0.0;
        return;
    }

    unsigned long long delta_total = total - prev_total;
    unsigned long long delta_idle = idle_time - prev_idle;
    prev_total = total;
    prev_idle = idle_time;

    if (delta_total == 0) {
        *cpu_usage = 0.0;
        return;
    }
    *cpu_usage = 100.0 * (1.0 - ((double)delta_idle / (double)delta_total));
    if (*cpu_usage < 0.0) *cpu_usage = 0.0;
    if (*cpu_usage > 100.0) *cpu_usage = 100.0;
}

void collect_metrics(struct Metrics *metrics) {
    FILE *fp = fopen("/proc/meminfo", "r");
    unsigned long mem_total = 0, mem_available = 0, mem_free = 0;
    int have_available = 0;
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) continue;
            if (sscanf(line, "MemAvailable: %lu kB", &mem_available) == 1) { have_available = 1; continue; }
            if (sscanf(line, "MemFree: %lu kB", &mem_free) == 1) continue;
        }
        fclose(fp);
    }
    unsigned long available = have_available ? mem_available : mem_free;
    if (mem_total > 0) {
        metrics->ram = 100.0 * (1.0 - ((double)available / (double)mem_total));
        if (metrics->ram < 0.0) metrics->ram = 0.0;
        if (metrics->ram > 100.0) metrics->ram = 100.0;
    } else {
        metrics->ram = 0.0;
    }

    struct statvfs st;
    if (statvfs("/", &st) == 0) {
        unsigned long long total_blocks = st.f_blocks;
        unsigned long long free_blocks = st.f_bfree;
        metrics->disk = 100.0 * (1.0 - ((double)free_blocks / (double)total_blocks));
        if (metrics->disk < 0.0) metrics->disk = 0.0;
        if (metrics->disk > 100.0) metrics->disk = 100.0;
    } else {
        metrics->disk = 0.0;
    }

    read_proc_stat(&metrics->cpu);
}