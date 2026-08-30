/*
 * print_queue.c - Mo phong hang doi in van phong (producer-consumer)
 *                 bang pthread + mutex + condition variable
 * ------------------------------------------------------------------
 * 3 thread "producer" (nguoi nop tai lieu) lien tuc dua tai lieu vao
 * hang doi dung chung. 1 thread "printer" (may in) lay tai lieu ra
 * de in, moi lan 1 tai lieu. Hang doi chi chua toi da 5 tai lieu.
 *
 * TAI SAO pthread_cond_wait() PHAI NAM TRONG VONG LAP while(),
 * KHONG PHAI if()?
 * ------------------------------------------------------------------
 * Cach dung (trong bai nay):
 *
 *      pthread_mutex_lock(&q_lock);
 *      while (count == 0 && !all_sent) {
 *          pthread_cond_wait(&not_empty, &q_lock);
 *      }
 *      ... dequeue ...
 *      pthread_mutex_unlock(&q_lock);
 *
 * Neu dung if() thay vi while():
 *
 *      pthread_mutex_lock(&q_lock);
 *      if (count == 0 && !all_sent) {
 *          pthread_cond_wait(&not_empty, &q_lock);
 *      }
 *      ... dequeue ...   <-- CO THE CHAY KHI count VAN LA 0!
 *      pthread_mutex_unlock(&q_lock);
 *
 * co 2 ly do khien dieu nay SAI:
 *
 * (1) "Spurious wakeup" (thuc tinh gia / thuc tinh khong ro nguyen
 *     nhan): Theo dinh nghia cua chuan POSIX, pthread_cond_wait()
 *     duoc PHEP tu dong tra ve (thuc tinh thread) MA KHONG CO AI
 *     goi pthread_cond_signal() hay pthread_cond_broadcast() ca.
 *     Day khong phai loi, ma la hanh vi duoc chuan cho phep vi ly
 *     do hieu nang/cai dat he thong (vi du de xu ly tin hieu OS,
 *     toi uu hoa tren mot so kien truc phan cung). Neu dung if(),
 *     khi co "thuc tinh gia" nay xay ra, thread se chay tiep xuong
 *     duoi ma DIEU KIEN (count == 0) van con dung -> printer se co
 *     gang doc/ghi vao hang doi dang RONG -> loi du lieu (vi du doc
 *     rac hoac truy cap ngoai vung nho).
 *
 * (2) "Lost wakeup do nhieu thread cung doi" (nay khong phai bai
 *     nay vi chi co 1 printer, nhung nguyen tac chung van ap dung
 *     cho phia producer - co 3 producer cung doi &not_full): khi
 *     mot dieu kien duoc bao (signal), CO THE CO NHIEU THREAD dang
 *     doi tren cung 1 condition variable duoc danh thuc, nhung chi
 *     MOT trong so do thuc su thoa man dieu kien (vi du chi con 1
 *     cho trong hang doi nhung co 2 producer cung tinh day vao).
 *     Neu dung if(), thread thuc day nhung khong con cho se van cu
 *     tien hanh ghi de len du lieu -> tran hang doi (buffer
 *     overflow / vi pham rang buoc count <= 5).
 *
 * => Vi vay LUON LUON phai kiem tra lai dieu kien SAU KHI thuc day,
 *    bang mot vong lap while(). Chi khi dieu kien thuc su dung
 *    (count > 0, hoac count < 5...) thi thread moi duoc phep di
 *    tiep. Day la quy tac bat buoc (mandatory idiom) khi lap trinh
 *    voi condition variable trong POSIX threads.
 * ------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define QUEUE_CAPACITY 5
#define DOCS_PER_PRODUCER 3
#define NUM_PRODUCERS 3
#define TOTAL_DOCS (NUM_PRODUCERS * DOCS_PER_PRODUCER)

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

Document queue[QUEUE_CAPACITY];
int head = 0, tail = 0, count = 0;
int all_sent = 0;   /* main dat = 1 sau khi da join xong tat ca producer */

int total_pages_printed = 0;
int total_docs_printed = 0;

pthread_mutex_t q_lock;
pthread_cond_t  not_full;    /* producer doi o day khi count == 5 */
pthread_cond_t  not_empty;   /* printer doi o day khi count == 0 */

/* Danh sach ten file + so trang mau, moi producer se lay tuan tu 3 file */
typedef struct { const char *name; int pages; } DocTemplate;

DocTemplate templates[NUM_PRODUCERS][DOCS_PER_PRODUCER] = {
    { {"report_Q1.pdf", 12}, {"slides.pdf", 20}, {"summary.pdf", 4} },
    { {"contract.pdf", 5},   {"memo.pdf", 2},     {"budget.pdf", 7} },
    { {"invoice.pdf", 3},    {"proposal.pdf", 8}, {"agenda.pdf", 6} }
};

void *producer(void *arg) {
    int id = *(int *)arg;
    static int global_doc_id = 1;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++) {
        pthread_mutex_lock(&q_lock);

        /* Neu hang doi day, doi cho den khi co cho trong */
        while (count == QUEUE_CAPACITY) {
            printf("[Producer %d] Queue full - waiting...\n", id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        /* --- enqueue, van dang giu q_lock --- */
        {
            int local_id;
            local_id = global_doc_id++;
            Document d;
            d.doc_id = local_id;
            strncpy(d.filename, templates[id - 1][i].name, sizeof(d.filename) - 1);
            d.filename[sizeof(d.filename) - 1] = '\0';
            d.pages = templates[id - 1][i].pages;

            queue[tail] = d;
            tail = (tail + 1) % QUEUE_CAPACITY;
            count++;

            printf("[Producer %d] Submitting: %-14s (%2d pages) - queue: %d/%d\n",
                   id, d.filename, d.pages, count, QUEUE_CAPACITY);
        }

        pthread_cond_signal(&not_empty);   /* bao cho printer biet co hang moi */
        pthread_mutex_unlock(&q_lock);

        usleep(300000); /* mo phong thoi gian soan tai lieu */
    }

    return NULL;
}

void *printer(void *arg) {
    (void)arg;

    while (1) {
        pthread_mutex_lock(&q_lock);

        /* Doi khi hang doi rong VA con producer chua gui xong */
        while (count == 0 && !all_sent) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        /* Neu hang doi rong VA tat ca producer da xong -> ket thuc */
        if (count == 0 && all_sent) {
            pthread_mutex_unlock(&q_lock);
            break;
        }

        /* --- dequeue --- */
        Document d = queue[head];
        head = (head + 1) % QUEUE_CAPACITY;
        count--;

        printf("[Printer]    Printing:   %-14s (%2d pages) - queue: %d/%d\n",
               d.filename, d.pages, count, QUEUE_CAPACITY);

        total_pages_printed += d.pages;
        total_docs_printed++;

        pthread_cond_signal(&not_full);    /* bao cho mot producer dang doi biet co cho trong */
        pthread_mutex_unlock(&q_lock);

        sleep(1); /* mo phong thoi gian in */
    }

    printf("[Printer]    All documents printed. Exiting.\n");
    return NULL;
}

int main(void) {
    pthread_t producer_threads[NUM_PRODUCERS];
    pthread_t printer_thread;
    int producer_ids[NUM_PRODUCERS] = {1, 2, 3};

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: %d documents\n", QUEUE_CAPACITY);
    printf("==============================================\n\n");

    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&printer_thread, NULL, printer, NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producer_threads[i], NULL, producer, &producer_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producer_threads[i], NULL);
    }

    /* Tat ca producer da xong -> bao cho printer biet se khong co
     * tai lieu moi nao nua, de no thoat vong lap khi hang doi rong */
    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&q_lock);

    pthread_join(printer_thread, NULL);

    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", TOTAL_DOCS);
    printf("  Documents printed   : %d\n", total_docs_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
