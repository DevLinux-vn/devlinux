#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_IP   "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

void read_cpu_load(double *load1) {
    FILE *f = fopen("/proc/loadavg", "r");
    if (!f) {
        *load1 = 0.0;
        return;
    }
    fscanf(f, "%lf", load1);
    fclose(f);
}

void read_mem_info(double *mem_used_pct) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) {
        *mem_used_pct = 0.0;
        return;
    }
    
    long mem_total = 0, mem_free = 0;
    char line[128];
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld", &mem_total);
        } else if (strncmp(line, "MemFree:", 8) == 0) {
            sscanf(line, "MemFree: %ld", &mem_free);
        }
    }
    fclose(f);
    
    if (mem_total > 0) {
        *mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;
    } else {
        *mem_used_pct = 0.0;
    }
}

int main() {
    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }
    
    // Configure collector address
    struct sockaddr_in collector_addr;
    memset(&collector_addr, 0, sizeof(collector_addr));
    collector_addr.sin_family = AF_INET;
    collector_addr.sin_port = htons(COLLECTOR_PORT);
    inet_pton(AF_INET, COLLECTOR_IP, &collector_addr.sin_addr);
    
    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);
    
    // Send 5 datagrams
    for (int i = 1; i <= 5; i++) {
        double load1, mem_used_pct;
        read_cpu_load(&load1);
        read_mem_info(&mem_used_pct);
        
        // Compute temperature: temp = 40 + load1 * 10
        double temp = 40.0 + load1 * 10.0;
        
        char message[BUFFER_SIZE];
        snprintf(message, sizeof(message), "id=sensor-01 temp=%.1f mem_used=%.1f%%", temp, mem_used_pct);
        
        if (sendto(sock, message, strlen(message), 0, (struct sockaddr *)&collector_addr, sizeof(collector_addr)) == -1) {
            perror("sendto");
            continue;
        }
        
        printf("[Sent %d/5] %s\n", i, message);
        
        if (i < 5) {
            sleep(2);
        }
    }
    
    close(sock);
    printf("[Sensor] Done.\n");
    
    return 0;
}
