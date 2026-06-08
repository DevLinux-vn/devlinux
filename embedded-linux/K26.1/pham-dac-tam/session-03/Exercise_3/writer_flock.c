/*
 * writer_flock.c - Multi-process logger using flock
 * 
 * Comparison with fcntl:
 * ├─ Lock granularity: Whole file only
 * ├─ Works over NFS: No
 * ├─ Inherited across fork: Yes (child gets a copy)
 * ├─ Automatically released: Yes (on close or crash)
 * └─ Best for: Simple local file locking
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <time.h>

#define LOG_FILE "system.log"
#define MAX_MSG 256

// Lấy timestamp hiện tại theo format: YYYY-MM-DD HH:MM:SS
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        exit(1);
    }
    
    const char *message = argv[1];
    pid_t pid = getpid();
    
    // Mở file với append mode, tạo nếu chưa tồn tại
    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    
    // --- CRITICAL SECTION BẮT ĐẦU ---
    // Acquire exclusive lock trên toàn bộ file
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock LOCK_EX");
        close(fd);
        exit(1);
    }
    
    // Format log line
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    char log_line[512];
    snprintf(log_line, sizeof(log_line), "[PID:%d] [%s] [INFO] %s\n", 
             pid, timestamp, message);
    
    // Ghi log line vào file
    if (write(fd, log_line, strlen(log_line)) == -1) {
        perror("write");
        flock(fd, LOCK_UN);
        close(fd);
        exit(1);
    }
    
    // Release lock
    if (flock(fd, LOCK_UN) == -1) {
        perror("flock LOCK_UN");
        close(fd);
        exit(1);
    }
    // --- CRITICAL SECTION KẾT THÚC ---
    
    close(fd);
    return 0;
}
