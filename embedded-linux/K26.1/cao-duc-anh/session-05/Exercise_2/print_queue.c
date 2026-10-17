#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define PRODUCER_NUM 3
#define QUEUE_SIZE 5
#define DOC_PER_PRODUCER 3

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

Document documents[PRODUCER_NUM*DOC_PER_PRODUCER] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",  1},
    {3, "Le Van Cuong",   3},
    {4, "Pham Thi Dung",  1},
    {5, "Hoang Van Em",   2},
    {6, "Cao Hoang Van",  20},
    {7, "Peter Pen",  1},
    {8, "Tong Duc Tung",   3},
    {9, "Haland",  10},
};

pthread_t threads[PRODUCER_NUM + 1];
Document queue[QUEUE_SIZE];
int document_id = 0;
int head = 0, tail = 0, count = 0;
int all_sent = 0;
int printed_count = 0;
int total_pages_printed = 0;

pthread_mutex_t lock;
pthread_cond_t  not_full;   /* producers wait here when count == QUEUE_SIZE */
pthread_cond_t  not_empty;  /* printer  waits here when count == 0 */

void *producer_handle(void *arg) {
    int producer_id = (int)(intptr_t)arg;

    for (int i = 0; i < DOC_PER_PRODUCER; i++) {
        sleep(2); // buộc các thread thực sự chạy song song trước khi vào critical section
        pthread_mutex_lock(&lock);
        while (count >= QUEUE_SIZE) {
            printf("[Producer %d] Queue full — waiting...\n", producer_id);
            pthread_cond_wait(&not_full, &lock);
        }

        int   doc_id = documents[document_id].doc_id;
        const char *fname = documents[document_id].filename;
        int   pages = documents[document_id].pages;

        queue[head].doc_id = doc_id;
        strncpy(queue[head].filename, fname, sizeof(queue[head].filename) - 1);
        queue[head].filename[sizeof(queue[head].filename) - 1] = '\0';
        queue[head].pages = pages;

        count++;
        document_id++;
        head = (head + 1) % QUEUE_SIZE;

        printf("[Producer %d] Submitting: %s (%d pages) — queue: %d/%d\n",
               producer_id, fname, pages, count, QUEUE_SIZE);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void *printer_handler(void *arg) {
    (void)arg;

    while ((all_sent == 0) || (count != 0)) {
        pthread_mutex_lock(&lock);

        /*
         * pthread_cond_wait() PHẢI nằm trong vòng lặp `while`, không phải `if`:
         * 1) Spurious wakeup: hệ điều hành có thể đánh thức một thread đang
         *    pthread_cond_wait() dù không hề có pthread_cond_signal()/broadcast()
         *    nào thực sự xảy ra. Nếu chỉ dùng `if`, thread sẽ chạy tiếp dù điều
         *    kiện (count == 0) vẫn còn đúng.
         * 2) Ngay cả khi được đánh thức "thật" (do có signal), giữa lúc thread
         *    tỉnh dậy và lúc nó thực sự giành lại được mutex, một thread khác
         *    có thể đã chen vào và làm điều kiện không còn đúng nữa. `while`
         *    đảm bảo điều kiện được kiểm tra lại ngay sau khi tỉnh dậy, trước
         *    khi coi là an toàn để tiếp tục.
         */
        while (count == 0) {
            if (all_sent == 1) break;
            pthread_cond_wait(&not_empty, &lock);
        }

        if (count == 0) {
            pthread_mutex_unlock(&lock);
            break;
        }

        Document doc = queue[tail];
        count--;
        tail = (tail + 1) % QUEUE_SIZE;

        printf("[Printer]    Printing:   %s (%d pages) — queue: %d/%d\n",
               doc.filename, doc.pages, count, QUEUE_SIZE);

        printed_count++;
        total_pages_printed += doc.pages;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&lock);
        sleep(5);
    }

    printf("[Printer]    All documents printed. Exiting.\n");

    return NULL;
}

int main() {
    pthread_t thread_id;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (%d producers, 1 printer)\n", PRODUCER_NUM);
    printf("   Queue capacity: %d documents\n", QUEUE_SIZE);
    printf("==============================================\n\n");

    for (int i = 0; i < PRODUCER_NUM; i++) {
        if (pthread_create(&thread_id, NULL, producer_handle, (void *)(intptr_t)(i + 1)) != 0) {
            perror("Lỗi không thể tạo thread");
            exit(1);
        }
        threads[i] = thread_id;
    }

    if (pthread_create(&thread_id, NULL, printer_handler, NULL) != 0) {
        perror("Lỗi không thể tạo thread");
        exit(1);
    }
    threads[PRODUCER_NUM] = thread_id;

    // Đợi 3 producer kết thúc trước
    for (int i = 0; i < PRODUCER_NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    all_sent = 1;
    pthread_cond_broadcast(&not_empty); // đánh thức printer nếu đang chờ, để nó kiểm tra all_sent
    pthread_join(threads[PRODUCER_NUM], NULL);

    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", PRODUCER_NUM * DOC_PER_PRODUCER);
    printf("  Documents printed   : %d\n", printed_count);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_empty);
    pthread_cond_destroy(&not_full);

    return 0;
}
