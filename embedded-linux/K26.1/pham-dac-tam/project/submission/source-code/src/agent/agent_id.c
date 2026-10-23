#include "../common.h"

int load_or_create_agent_id(const char *path, char *agent_id, size_t size) {
    FILE *fp = fopen(path, "r");
    if (fp != NULL) {
        if (fgets(agent_id, (int)size, fp) != NULL) {
            agent_id[strcspn(agent_id, "\r\n")] = '\0';
            fclose(fp);
            return 1;
        }
        fclose(fp);
    }

    char host[128] = {0};
    gethostname(host, sizeof(host));
    snprintf(agent_id, size, "%s-%ld", host, (long)getpid());

    fp = fopen(path, "w");
    if (fp != NULL) {
        fprintf(fp, "%s\n", agent_id);
        fclose(fp);
    }
    return 0;
}