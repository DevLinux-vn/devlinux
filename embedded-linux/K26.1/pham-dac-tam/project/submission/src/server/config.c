#include "../common.h"

void set_default_config(struct Config *cfg) {
    if (!cfg) return;
    cfg->interval = 3;
    cfg->cpu_warning = 70.0;
    cfg->cpu_critical = 90.0;
    cfg->ram_warning = 75.0;
    cfg->ram_critical = 90.0;
    cfg->disk_warning = 80.0;
    cfg->disk_critical = 95.0;
}
