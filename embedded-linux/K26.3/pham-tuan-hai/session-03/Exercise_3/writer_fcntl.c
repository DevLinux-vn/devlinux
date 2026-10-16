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
#include <errno.h>

static int write_full(int fd, const void *buf, size_t count) {
  size_t total_written = 0;
  const char *ptr = (const char *)buf;
  
  while (total_written < count) {
    ssize_t written = write(fd, ptr + total_written,
                              count - total_written);
    if (written < 0) {
      if(errno == EINTR) continue;
      
      return -1;
    }
    total_written += (size_t)written;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  //Kiem tra dau vao
  if(argc < 2) {
    printf("Use %s \"your message\" ",argv[0]);
    return 1;
  }
  
  //Lay PID
  pid_t pid = getpid();
  
  //Lay thoi gian + dinh dang
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
   
  //Tao chuoi theo dinh dang
  char log_buffer[512];
  int len = snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", pid, time_str, argv[1]);
  
  if(len < 0 || len >= (int)sizeof(log_buffer)) {
    fprintf(stderr, "Log message formatting error or truncated\n");
    return 1;
  }
  //Mo file system.log
  int fd = open("system.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
  if (fd < 0) {
    printf("Mo file that bai\n");
    return 1;
  }
  
  //Khoa file
  struct flock fl = {
    .l_type = F_WRLCK,
    .l_whence = SEEK_SET,
    .l_start = 0,
    .l_len = 0,
  }; 
  
  if(fcntl(fd, F_SETLKW, &fl) == -1) {
    perror("Loi khoa file");
    if (close(fd) == -1) {
          perror("close failed");
          return 1;
    }
    return 1;
  }
  
  //Ghi file
  if(write_full(fd, log_buffer, len) == -1) {
    printf("Write failed\n");
    return 1;
  }
  
  if(fsync(fd) == -1) {
    perror("fsync failed\n");
  }
  
  //Mo khoa file
  fl.l_type = F_UNLCK;
  fcntl(fd, F_SETLK, &fl);
  
  if (close(fd) == -1) {
    perror("close failed");
    return 1;
  }
  return 0;
}