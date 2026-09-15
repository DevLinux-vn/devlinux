#include "../common.h"

static pthread_mutex_t g_metrics_lock = PTHREAD_MUTEX_INITIALIZER;
static struct Metrics g_metrics;
static struct Config g_config;
static volatile sig_atomic_t g_running = 1;
static int g_connected;

static void *collector_thread(void *arg) {
    (void)arg;
    while (g_running) {
        pthread_mutex_lock(&g_metrics_lock);
        collect_metrics(&g_metrics);
        pthread_mutex_unlock(&g_metrics_lock);
        sleep(1);
    }
    return NULL;
}

static void *dashboard_thread(void *arg) {
    const char *agent_id = (const char *)arg;
    while (g_running) {
        struct Metrics snapshot;
        struct Config config_snapshot;
        int connected;
        pthread_mutex_lock(&g_metrics_lock);
        snapshot = g_metrics;
        config_snapshot = g_config;
        connected = g_connected;
        pthread_mutex_unlock(&g_metrics_lock);

        render_agent_dashboard(agent_id, &snapshot, &config_snapshot, connected, connected ? STATUS_ONLINE : STATUS_OFFLINE);
        sleep(2);
    }
    return NULL;
}

static int connect_to_server(const char *host, int port){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        close(sock);
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

int main(int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
    const char *host = (argc > 1) ? argv[1] : DEFAULT_HOST;
    int port = (argc > 2) ? atoi(argv[2]) : DEFAULT_PORT;
    char agent_id[64] = {0};
    load_or_create_agent_id(AGENT_ID_FILE, agent_id, sizeof(agent_id));

    pthread_mutex_lock(&g_metrics_lock);
    memset(&g_metrics, 0, sizeof(g_metrics));
    set_default_config(&g_config);
    collect_metrics(&g_metrics);
    g_connected = 0;
    pthread_mutex_unlock(&g_metrics_lock);

    pthread_t dash_tid;
    pthread_t collector_tid;
    pthread_create(&dash_tid, NULL, dashboard_thread, (void *)agent_id);
    pthread_create(&collector_tid, NULL, collector_thread, NULL);

    while (g_running) {
        int sock = connect_to_server(host, port);
        if (sock < 0) {
            perror("connect");
            sleep(5);
            continue;
        }
        enable_keepalive(sock, 5, 2, 3);
        pthread_mutex_lock(&g_metrics_lock);
        g_connected = 1;
        pthread_mutex_unlock(&g_metrics_lock);

        time_t next_send = time(NULL);
        while (g_running) {
            int interval;
            pthread_mutex_lock(&g_metrics_lock);
            interval = g_config.interval;
            pthread_mutex_unlock(&g_metrics_lock);
            if (interval < 1) interval = HEARTBEAT_SEC;

            struct timeval timeout = {1, 0};
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
            if (select(sock + 1, &rfds, NULL, NULL, &timeout) > 0 && FD_ISSET(sock, &rfds)) {
            char incoming[MAX_LINE];
            ssize_t nb = recv(sock, incoming, sizeof(incoming) - 1, 0);
                if (nb <= 0) break;
            incoming[nb] = '\0';

            struct Message msg;
            if (parse_message(incoming, &msg)) {
                if (strcmp(msg.type, "config") == 0) {
                    char *end = NULL;
                    double threshold_value = strtod(msg.value, &end);
                    int has_valid_threshold = (end != msg.value && *end == '\0' && threshold_value >= 0.0 && threshold_value <= 100.0);
                    int new_interval = atoi(msg.value);
                    pthread_mutex_lock(&g_metrics_lock);
                    if (strcmp(msg.key, "interval") == 0 && new_interval >= 1 && new_interval <= 300) {
                        g_config.interval = new_interval;
                    } else if (has_valid_threshold && strcmp(msg.key, "cpu_warning") == 0) {
                        g_config.cpu_warning = threshold_value;
                    } else if (has_valid_threshold && strcmp(msg.key, "cpu_critical") == 0) {
                        g_config.cpu_critical = threshold_value;
                    } else if (has_valid_threshold && strcmp(msg.key, "ram_warning") == 0) {
                        g_config.ram_warning = threshold_value;
                    } else if (has_valid_threshold && strcmp(msg.key, "ram_critical") == 0) {
                        g_config.ram_critical = threshold_value;
                    } else if (has_valid_threshold && strcmp(msg.key, "disk_warning") == 0) {
                        g_config.disk_warning = threshold_value;
                    } else if (has_valid_threshold && strcmp(msg.key, "disk_critical") == 0) {
                        g_config.disk_critical = threshold_value;
                    }
                    pthread_mutex_unlock(&g_metrics_lock);
                    char ack[128];
                    format_ack_message(ack, sizeof(ack), "ok");
                    send(sock, ack, strlen(ack), 0);
                }
            }
        }

            if (time(NULL) >= next_send) {
                char buf[MAX_LINE];
                struct Metrics local;
                pthread_mutex_lock(&g_metrics_lock);
                local = g_metrics;
                interval = g_config.interval;
                pthread_mutex_unlock(&g_metrics_lock);
                snprintf(buf, sizeof(buf), "{\"type\":\"data\",\"agent_id\":\"%s\",\"cpu\":%.1f,\"ram\":%.1f,\"disk\":%.1f}\n", agent_id, local.cpu, local.ram, local.disk);
                if (send(sock, buf, strlen(buf), 0) < 0) break;
                char heartbeat[MAX_LINE];
                format_heartbeat_message(heartbeat, sizeof(heartbeat), agent_id);
                if (send(sock, heartbeat, strlen(heartbeat), 0) < 0) break;
                next_send = time(NULL) + (interval > 0 ? interval : HEARTBEAT_SEC);
            }
        }
        close(sock);
        pthread_mutex_lock(&g_metrics_lock);
        g_connected = 0;
        pthread_mutex_unlock(&g_metrics_lock);
        sleep(5);
    }

    g_running = 0;
    pthread_join(collector_tid, NULL);
    pthread_join(dash_tid, NULL);
    return 0;
}