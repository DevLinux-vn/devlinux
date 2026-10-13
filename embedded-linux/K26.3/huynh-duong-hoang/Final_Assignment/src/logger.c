#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "gateway.h"
#include <time.h>


void logger_send(const char *source, const char *message){
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd < 0){
        return;
    }

    char buffer[512];
    int len = snprintf(buffer, 512, "%s %s\n", source, message);
    if (len > 0){
        write(fd, buffer, len);
    }
    close(fd);
}

void run_logger(volatile sig_atomic_t *stop){
    mkfifo(FIFO_NAME, 0666);
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd < 0){
        perror("open fifo failed");
        exit(EXIT_FAILURE);
    }
    
    FILE *log_file = fopen(LOG_FILE, "w");
    if (log_file == NULL){
        perror("open log file failed");
        exit(EXIT_FAILURE);
    }

    unsigned long seq = 1;
    char read_buffer[512];

    while (!*stop){
        ssize_t n = read(fd, read_buffer, 512-1);
        if (n > 0){
            read_buffer[n] = '\0';
            char *line = strtok(read_buffer, "\n");
            while (line != NULL && line[0] != '\0'){
                time_t now = time(NULL);
                struct tm *tm_info = localtime(&now);
                char time_str[32];
                strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", tm_info);

                fprintf(log_file, "%lu %s %s\n", seq++, time_str, line);
                fflush(log_file);

                line = strtok(NULL, "\n");
            }
        }else{
            usleep(50000);
        }
    }
    fclose(log_file);
    close(fd);
    exit(EXIT_SUCCESS);
}