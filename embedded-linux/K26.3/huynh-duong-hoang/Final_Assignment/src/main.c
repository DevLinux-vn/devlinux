#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "gateway.h"
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

void run_logger(volatile sig_atomic_t *stop);

static gateway_t gateway;

void handle_signal(int sig){
    if (sig == SIGINT || sig == SIGTERM){
        gateway.stop = 1;
    }
}

void *connection_manager_thread(void *arg){
    gateway_t *gateway = (gateway_t *)arg;
#ifdef DEBUG
    printf("Connection manager thread started\n");
#endif
    while (!gateway->stop){
        usleep(100000);
    }
    return NULL;
}

void *data_manager_thread(void *arg){
    gateway_t *gateway = (gateway_t *)arg;
#ifdef DEBUG
    printf("Data manager thread started\n");
#endif
    while (!gateway->stop){
        usleep(100000);
    }
    return NULL;
}

void *storage_manager_thread(void *arg){
    gateway_t *gateway = (gateway_t *)arg;
#ifdef DEBUG
    printf("Storage manager thread started\n");
#endif
    while (!gateway->stop){
        usleep(100000);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc<3){
        fprintf(stderr, "Usage : %s <base port P> <http port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    gateway.base_port = (uint16_t)atoi(argv[1]);
    gateway.http_port = (uint16_t)atoi(argv[2]);
    gateway.stop = 0;

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    mkfifo(FIFO_NAME, 0666);

    pid_t log_pid = fork();
    if (log_pid < 0){
        perror("fork log process failed");
        exit(EXIT_FAILURE);
    }else if (log_pid == 0){
        run_logger(&gateway.stop);
    }
    gateway.log_pid = log_pid;

    char start_message[128];
    snprintf(start_message, 128, "Gateway started: \nTCP base port %d, HTTP port %d", gateway.base_port, gateway.http_port);
    logger_send("main", start_message);

    printf("Gateway started: \nTCP base port %d \nTCP http port %d\n", gateway.base_port, gateway.http_port);

    pthread_t connection_tid, data_tid, storage_tid;
    pthread_create(&connection_tid, NULL, connection_manager_thread, &gateway);
    pthread_create(&data_tid, NULL, data_manager_thread, &gateway);
    pthread_create(&storage_tid, NULL, storage_manager_thread, &gateway);

    while (!gateway.stop){
        pause();
    }

    logger_send("main", "Gateway stopped");

    printf("\nGateway stopped\n");

    pthread_join(connection_tid, NULL);
    pthread_join(data_tid, NULL);    
    pthread_join(storage_tid, NULL);

    kill(gateway.log_pid, SIGTERM);
    waitpid(gateway.log_pid, NULL, 0);
    unlink(FIFO_NAME);

    return 0;
}
