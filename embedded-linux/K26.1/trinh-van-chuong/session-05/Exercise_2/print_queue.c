/*
 * print_queue.c — Mô phỏng hàng đợi in ấn văn phòng (3 Producers, 1 Printer)
 *
 * Biên dịch bằng Makefile (hoặc lệnh: gcc -Wall -pthread -o print_queue print_queue.c)
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

/* Cấu hình hàng đợi dùng chung */
Document queue[5];
int head = 0;       // Vị trí lấy tài liệu ra (Printer dùng)
int tail = 0;       // Vị trí thêm tài liệu vào (Producer dùng)
int count = 0;      // Số lượng tài liệu hiện có trong queue [0, 5]
int all_sent = 0;   // Cờ hiệu báo tất cả producer đã nộp bài xong (0: chưa xong, 1: đã xong)

/* Công cụ đồng bộ hóa */
pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  not_full = PTHREAD_COND_INITIALIZER;   // Đợi khi queue đầy (count == 5)
pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;  // Đợi khi queue rỗng (count == 0)

/* Các biến toàn cục để tính bảng tổng kết (Summary) */
int total_submitted = 0;
int total_printed = 0;
int total_pages_printed = 0;

/* Hàm thực hiện việc thêm tài liệu vào hàng đợi vòng (Circular Queue) */
void enqueue(Document doc) {
    queue[tail] = doc;
    tail = (tail + 1) % 5; // Tịnh tiến tail trong mảng vòng 5 phần tử
    count++;
}

/* Hàm thực hiện việc lấy tài liệu ra khỏi hàng đợi vòng */
Document dequeue() {
    Document doc = queue[head];
    head = (head + 1) % 5; // Tịnh tiến head trong mảng vòng 5 phần tử
    count--;
    return doc;
}

/* Luồng Nhà sản xuất (Producer Threads) */
void* producer(void* arg) {
    int p_id = *(int*)arg;
    int i;
    
    // Tên các file giả lập để nộp in
    char* filenames[3][3] = {
        {"report_Q1.pdf", "slides.pdf", "summary.pdf"},
        {"contract.pdf", "memo.pdf", "budget.pdf"},
        {"invoice.pdf", "proposal.pdf", "plan.txt"}
    };
    int pages_sample[3][3] = {
        {12, 20, 4},
        {5, 2, 7},
        {3, 8, 5}
    };

    // Mỗi producer gửi đúng 3 tài liệu
    for (i = 0; i < 3; i++) {
        pthread_mutex_lock(&q_lock);

        // Nếu hàng đợi đầy, phải đi ngủ đợi tín hiệu 'not_full' từ Printer
        while (count == 5) {
            printf("[Producer %d] Queue full — waiting...\n", p_id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        // Tạo dữ liệu tài liệu
        Document doc;
        doc.doc_id = (p_id * 10) + i; // Định danh ID độc nhất cho mỗi tài liệu
        strcpy(doc.filename, filenames[p_id - 1][i]);
        doc.pages = pages_sample[p_id - 1][i];

        // Đưa vào hàng đợi
        enqueue(doc);
        total_submitted++;
        printf("[Producer %d] Submitting: %s (%d pages) — queue: %d/5\n", 
               p_id, doc.filename, doc.pages, count);

        // Bấm chuông báo cho Printer biết đã có hàng để in
        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);
        
        // Ngủ nhẹ một chút để tạo sự xen kẽ giữa các Producer
        usleep(100000); 
    }

    return NULL;
}

/* Luồng Máy in (Printer Thread) */
void* printer(void* arg) {
    while (1) {
        pthread_mutex_lock(&q_lock);

        // Nếu queue rỗng VÀ chưa gửi xong hết tài liệu -> Đi ngủ đông
        while (count == 0 && !all_sent) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        // Điều kiện thoát: Queue đã rỗng hoàn toàn VÀ các producer đã nghỉ hết
        if (count == 0 && all_sent) {
            pthread_mutex_unlock(&q_lock);
            break; // Thoát hẳn vòng lặp, tắt máy in
        }

        // Lấy tài liệu ra in
        Document doc = dequeue();
        printf("[Printer]    Printing:   %s (%d pages) — queue: %d/5\n", 
               doc.filename, doc.pages, count);
               
        total_printed++;
        total_pages_printed += doc.pages;

        // Bấm chuông báo cho các Producer đang ngủ biết là hàng đợi đã trống bớt 1 chỗ
        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&q_lock);

        // Giả lập thời gian máy in đang chạy (1 giây/tài liệu)
        sleep(1); 
    }

    printf("[Printer]    All documents printed. Exiting.\n");
    return NULL;
}

int main(void) {
    pthread_t prod1, prod2, prod3, print_th;
    int id1 = 1, id2 = 2, id3 = 3;

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    // 1. Khởi tạo luồng Printer (Máy in) và 3 luồng Producer (Nhân viên gửi in)
    pthread_create(&print_th, NULL, printer, NULL);
    pthread_create(&prod1, NULL, producer, &id1);
    pthread_create(&prod2, NULL, producer, &id2);
    pthread_create(&prod3, NULL, producer, &id3);

    // 2. Thu gom xác của 3 ông nhân viên trước (Đợi họ gửi xong hết 9 tài liệu)
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(prod3, NULL);

    // 3. Khi 3 producer đã xong, khóa mutex để bật cờ báo tử 'all_sent = 1'
    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    // Đánh thức máy in dậy nếu nó lỡ đang ngủ đông ở hàng đợi trống, để nó check lại điều kiện và thoát
    pthread_cond_broadcast(&not_empty); 
    pthread_mutex_unlock(&q_lock);

    // 4. Chờ máy in xử lý nốt đống tài liệu tồn đọng và tắt hẳn
    pthread_join(print_th, NULL);

    // 5. In bảng tổng kết
    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", total_submitted);
    printf("  Documents printed   : %d\n", total_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    return 0;
}

/* ============================================================================
 * COMMENT BLOCK: GIẢI THÍCH VỀ VÒNG LẶP WHILE VÀ HIỆN TƯỢNG THỨC GIẤC GIẢ
 *
 * 1. Tại sao phải dùng vòng lặp 'while' thay vì câu lệnh 'if' khi gọi wait()?
 * -> Khi một luồng gọi hàm pthread_cond_wait(), nó sẽ đi ngủ. Đến khi được luồng khác
 * gọi dậy bằng lệnh signal(), luồng đó sẽ tỉnh giấc. 
 * -> Tuy nhiên, tại thời điểm nó tỉnh dậy và giành lại được khóa Mutex, ĐIỀU KIỆN 
 * CHƯA CHẮC ĐÃ ĐÚNG NỮA!
 * Ví dụ: Queue đang trống (count == 0). Printer đi ngủ ở câu lệnh wait. 
 * Producer 1 tạo 1 tài liệu, bỏ vào queue (count thành 1) và phát lệnh signal().
 * Printer thức giấc. Nhưng cùng lúc đó, có một Printer số 2 chạy nhanh hơn lao vào 
 * ăn mất tài liệu vừa tạo (count lại về 0). Khi Printer 1 thực sự cầm được khóa mutex 
 * để chạy, tài liệu đã không còn!
 * -> Nếu dùng 'if', Printer 1 sẽ chạy thẳng xuống dòng dequeue() và lấy dữ liệu rác 
 * ở hàng đợi trống -> Gây lỗi sập chương trình.
 * -> Bằng cách dùng 'while', khi luồng tỉnh dậy, nó bị ép buộc quay lại đầu vòng lặp 
 * để tự tay kiểm tra lại biến 'count' một lần nữa. Nếu điều kiện vẫn sai, nó sẽ 
 * ngoan ngoãn đi ngủ tiếp.
 *
 * 2. Hiện tượng Thức giấc giả (Spurious Wakeup) là gì?
 * -> Thức giấc giả là một hiện tượng kỳ lạ ở tầng nhân hệ điều hành (OS/Linux kernel). 
 * Đôi khi, một luồng đang nằm ngủ ở hàm pthread_cond_wait() tự nhiên bị đánh thức 
 * dậy một cách ngẫu nhiên mà KHÔNG HỀ có bất kỳ luồng nào gọi lệnh signal() hay 
 * broadcast().
 * -> Đây không phải là lỗi code của chúng ta, mà đó là do cách tối ưu hóa hiệu năng 
 * phần cứng và xử lý ngắt (interrupts) của hệ điều hành.
 * -> Do hệ điều hành không cam kết 100% luồng chỉ tỉnh khi có signal, nên việc bọc 
 * pthread_cond_wait() bằng vòng lặp 'while' chính là tấm khiên bảo vệ tuyệt đối, 
 * giúp lọc bỏ toàn bộ những lần "thức giấc giả" này.
 * ============================================================================ */