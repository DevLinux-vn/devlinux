#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define LOG_FILE "system.log"


int main(int argc, char *argv[])
{
    int fd;
    char buffer[256];
    time_t now;
    struct tm *time_info;
    struct flock lock;


    if (argc != 2)
    {
        printf("Usage: ./writer_fcntl \"message\"\n");
        return 1;
    }


    fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);

    if (fd < 0)
    {
        printf("Cannot open log file\n");
        return 1;
    }


    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;


    fcntl(fd, F_SETLKW, &lock);


    now = time(NULL);
    time_info = localtime(&now);


    snprintf(buffer, sizeof(buffer),
             "[PID:%d] [%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",
             getpid(),
             time_info->tm_year + 1900,
             time_info->tm_mon + 1,
             time_info->tm_mday,
             time_info->tm_hour,
             time_info->tm_min,
             time_info->tm_sec,
             argv[1]);


    write(fd, buffer, strlen(buffer));


    lock.l_type = F_UNLCK;


    fcntl(fd, F_SETLK, &lock);


    close(fd);

    return 0;
}