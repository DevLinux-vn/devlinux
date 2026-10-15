/*
Property                flock                       fcntl
Lock granularity        Whole file only             Byte range supported
Works over NFS          No                          Yes
Inherited across fork   Yes                         No
Best used when          Simple local file locking   Network FS or byte-range locking
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>

int main(int argc, char *argv[]) {
  // Kiem tra chuoi dau vao
  if (argc < 2) {
    printf("Use: %s \"message text\"\n", argv[0]);
    return 1;
  }
  
  //Lay current PID
  pid_t pid = getpid();
  
  //Lay thoi gian + dinh dang
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
  
  //Tao chuoi log theo dinh dang
  char log_buffer[512];
  int len = snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", pid, time_str, argv[1]);
  
  //Mo file system.log
  int fd = open("system.log", O_WRONLY | O_APPEND | O_CREAT, 0666);
  if(fd < 0) {
    perror("Mo file that bai!\n");
    return 1;
  }
  
  //Khoa file bang flock
  if (flock(fd, LOCK_EX) == -1) {
    close(fd);
    return 1;
  }
  
  write(fd, log_buffer, len);
  
  flock(fd, LOCK_UN);
  
  close(fd);
  return 0;
}