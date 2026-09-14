#include "../common.h"

static void read_proc_stat(double *cpu_usage) {
    FILE *fp = fopen("/proc/stat", "r");
    if(!fp) return;
    char line[256];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return;
    }
    fclose(fp);

    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    if (sscanf(line, "cpu  %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq) != 7) {
        *cpu_usage = 0.0;
        return;
    }
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq;
    unsigned long long idle_time = idle + iowait;
    *cpu_usage = 100.0 * (1.0 - ((double)idle_time / (double)total));
}

void collect_metrics(struct Metrics *metrics) {
    FILE *fp = fopen("/proc/meminfo", "r");
    unsigned long mem_total = 0, mem_avaiable = 0;
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "MemTotal: %lu kB", &mem_total) == 1) continue;
            if (sscanf(line, "MemAvailable: %lu kB", &mem_avaiable) == 1) break;
        }
        fclose(fp);
    }
    if (mem_total > 0) {
        metrics->ram = 100.0 * (1.0 - ((double)mem_avaiable / (double)mem_total));
    } else {
        metrics->ram = 0.0;
    }

    struct statvfs st;
    if (statvfs("/", &st) == 0) {
        unsigned long long total_blocks = st.f_blocks;
        unsigned long long free_blocks = st.f_bfree;
        metrics->disk = 100.0 * (1.0 - ((double)free_blocks / (double)total_blocks));
    } else {
        metrics->disk = 0.0;
    }

    read_proc_stat(&metrics->cpu);
}