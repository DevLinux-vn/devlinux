#include "../common.h"

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
    const char *host = (argc > 1) ? argv[1] : DEFAULT_HOST;
    int port = (argc > 2) ? atoi(argv[2]) : DEFAULT_PORT;
    char agent_id[64] = {0};
    load_or_create_agent_id(AGENT_ID_FILE, agent_id, sizeof(agent_id));

    int sock = connect_to_server(host, port);
    if (sock < 0) {
        perror("connect");
        return -1;
    }
    enable_keepalive(sock, 5, 2, 3);

    struct Metrics metrics;
    memset(&metrics, 0, sizeof(metrics));
    collect_metrics(&metrics);
    render_agent_dashboard(agent_id, &metrics, 1, STATUS_ONLINE);

    int interval = HEARTBEAT_SEC;
    char buf[MAX_LINE];
    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        struct timeval tv = {0, 0};
        if(select(sock + 1, &rfds, NULL, NULL, &tv) > 0 && FD_ISSET(sock, &rfds)) {
            char incoming[MAX_LINE];
            ssize_t nb = recv(sock, incoming, sizeof(incoming) - 1, 0);
            if (nb <= 0) break;
            incoming[nb] = '\0';
            // Process incoming message here
            struct Message msg;
            if(parse_message(incoming, &msg) && strcmp(msg.type, "status") == 0) {
                // Handle status message here
                interval = atoi(msg.value);
                if (interval <= 0) {
                    interval = HEARTBEAT_SEC;
                }
                char ack[128];
                format_ack_message(ack, sizeof(ack), "ok");
                send(sock, ack, strlen(ack), 0);
            }
        }

        collect_metrics(&metrics);
        render_agent_dashboard(agent_id, &metrics, 1, STATUS_ONLINE);
        snprintf(buf, sizeof(buf), "{\"type\":\"data\",\"agent_id\":\"%s\",\"cpu\":%.1f,\"ram\":%.1f,\"disk\":%.1f}\n", agent_id, metrics.cpu, metrics.ram, metrics.disk);
        send(sock, buf, strlen(buf), 0);

        char heartbeat[MAX_LINE];
        format_heartbeat_message(heartbeat, sizeof(heartbeat), agent_id);
        send(sock, heartbeat, strlen(heartbeat), 0);

        sleep(interval);
    }
    return 0;
}