/*
Why pthread_cond_wait() must be inside a while loop:

pthread_cond_wait() can wake up even when no thread signaled the condition.
This is called a spurious wakeup.

Also, even if a signal really happened, another thread may acquire the mutex
first and change the shared state before this thread continues.

Therefore, after waking up, the thread must re-check the condition.

Wrong:

    if (count == 0)
        pthread_cond_wait(...);

Correct:

    while (count == 0)
        pthread_cond_wait(...);

The while loop guarantees the thread only continues when the queue state is
really valid.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define QUEUE_SIZE 5
#define PRODUCER_COUNT 3
#define DOCS_PER_PRODUCER 3
#define TOTAL_DOCS (PRODUCER_COUNT * DOCS_PER_PRODUCER)

typedef struct {
    int doc_id;
    char filename[60];
    int pages;
} Document;

typedef struct {
    int producer_id;
    Document docs[DOCS_PER_PRODUCER];
} ProducerData;

Document queue[QUEUE_SIZE];

int head = 0;
int tail = 0;
int count = 0;
int all_sent = 0;

int documents_submitted = 0;
int documents_printed = 0;
int total_pages_printed = 0;

pthread_mutex_t q_lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

void enqueue(Document doc) {
    queue[tail] = doc;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
}

Document dequeue(void) {
    Document doc = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;
    return doc;
}

void *producer(void *arg) {
    ProducerData *data = (ProducerData *)arg;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++) {
        Document doc = data->docs[i];

        pthread_mutex_lock(&q_lock);

        while (count == QUEUE_SIZE) {
            printf("[Producer %d] Queue full — waiting...\n",
                   data->producer_id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        enqueue(doc);
        documents_submitted++;

        printf("[Producer %d] Submitting: %-14s (%d pages) — queue: %d/%d\n",
               data->producer_id,
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&q_lock);

        sleep(1);
    }

    return NULL;
}

void *printer(void *arg) {
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

        Document doc = dequeue();

        printf("[Printer]    Printing:   %-14s (%d pages) — queue: %d/%d\n",
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&q_lock);

        sleep(1);

        documents_printed++;
        total_pages_printed += doc.pages;
    }

    printf("[Printer]    All documents printed. Exiting.\n");

    return NULL;
}

int main(void) {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t printer_thread;

    ProducerData producer_data[PRODUCER_COUNT] = {
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

    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    printf("\n==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    pthread_create(&printer_thread, NULL, printer, NULL);

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer, &producer_data[i]);
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
    printf("  Documents submitted : %d\n", documents_submitted);
    printf("  Documents printed   : %d\n", documents_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    pthread_mutex_destroy(&q_lock);

    return 0;
}