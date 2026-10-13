#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 5
#define PRODUCER_COUNT 3
#define DOCS_PER_PRODUCER 3
#define TOTAL_DOCS (PRODUCER_COUNT * DOCS_PER_PRODUCER)

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

Document queue[QUEUE_SIZE];

int head = 0;
int tail = 0;
int count = 0;

int all_sent = 0;
int documents_printed = 0;
int total_pages_printed = 0;

pthread_mutex_t q_lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

/*
 * Why must pthread_cond_wait() be used inside a while loop?
 *
 * A thread can wake up even though the condition it is waiting for
 * is still false. This is called a "spurious wakeup".
 *
 * Also, multiple threads may wake up at nearly the same time and
 * compete for the mutex. Another thread may consume the resource
 * before the current thread acquires the lock again.
 *
 * Therefore, after every wakeup, the condition must be re-checked.
 *
 * Correct:
 *     while (condition_not_satisfied)
 *         pthread_cond_wait(...);
 *
 * Incorrect:
 *     if (condition_not_satisfied)
 *         pthread_cond_wait(...);
 */

typedef struct {
    int producer_id;
    Document docs[DOCS_PER_PRODUCER];
} ProducerData;

void enqueue(Document doc)
{
    queue[tail] = doc;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
}

Document dequeue_doc(void)
{
    Document doc = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;
    return doc;
}

void *producer(void *arg)
{
    ProducerData *pdata = (ProducerData *)arg;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++) {

        pthread_mutex_lock(&q_lock);

        while (count == QUEUE_SIZE) {
            printf("[Producer %d] Queue full - waiting...\n",
                   pdata->producer_id);

            pthread_cond_wait(&not_full, &q_lock);
        }

        enqueue(pdata->docs[i]);

        printf("[Producer %d] Submitting: %-15s (%2d pages) - queue: %d/%d\n",
               pdata->producer_id,
               pdata->docs[i].filename,
               pdata->docs[i].pages,
               count,
               QUEUE_SIZE);

        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);

        usleep(100000);
    }

    return NULL;
}

void *printer(void *arg)
{
    (void)arg;

    while (1) {

        pthread_mutex_lock(&q_lock);

        while (count == 0 && !all_sent) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        if (count == 0 && all_sent) {
            pthread_mutex_unlock(&q_lock);
            break;
        }

        Document doc = dequeue_doc();

        pthread_cond_signal(&not_full);

        printf("[Printer]    Printing:  %-15s (%2d pages) - queue: %d/%d\n",
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        pthread_mutex_unlock(&q_lock);

        sleep(1);

        documents_printed++;
        total_pages_printed += doc.pages;
    }

    printf("[Printer]    All documents printed. Exiting.\n");

    return NULL;
}

int main(void)
{
    pthread_t producers[PRODUCER_COUNT];
    pthread_t printer_thread;

    ProducerData pdata[PRODUCER_COUNT] = {
        {
            1,
            {
                {1, "report_Q1.pdf", 12},
                {2, "slides.pdf", 20},
                {3, "summary.pdf", 4}
            }
        },
        {
            2,
            {
                {4, "contract.pdf", 5},
                {5, "memo.pdf", 2},
                {6, "budget.pdf", 7}
            }
        },
        {
            3,
            {
                {7, "invoice.pdf", 3},
                {8, "proposal.pdf", 8},
                {9, "schedule.pdf", 5}
            }
        }
    };

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&printer_thread, NULL, printer, NULL);

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i],
                       NULL,
                       producer,
                       &pdata[i]);
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }

    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&q_lock);

    pthread_join(printer_thread, NULL);

    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", TOTAL_DOCS);
    printf("  Documents printed   : %d\n", documents_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}