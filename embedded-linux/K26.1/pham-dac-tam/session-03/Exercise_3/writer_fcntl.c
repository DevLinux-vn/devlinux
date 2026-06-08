/*
 * writer_fcntl.c - Multi-process logger using fcntl
 * 
 * Comparison with flock:
 * ├─ Lock granularity: Byte range supported (more fine-grained)
 * ├─ Works over NFS: Yes
 * ├─ Inherited across fork: No (per open-file-description)
 * ├─ Automatically released: Yes (on close or crash)
 * └─ Best for: Network FS or byte-range locking
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
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
    // Setup fcntl lock structure
    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type   = F_WRLCK;      // Write lock (exclusive)
    fl.l_whence = SEEK_SET;     // Relative to start of file
    fl.l_start  = 0;            // Start offset
    fl.l_len    = 0;            // 0 = lock entire file
    
    // Acquire exclusive lock (F_SETLKW = blocking)
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl F_SETLKW");
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
        // Release lock on error
        fl.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &fl);
        close(fd);
        exit(1);
    }
    
    // Release lock
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl F_SETLK");
        close(fd);
        exit(1);
    }
    // --- CRITICAL SECTION KẾT THÚC ---
    
    close(fd);
    return 0;
}
