#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_SIZE 5
#define NUM_PRODUCERS 3
#define DOCS_PER_PRODUCER 3
#define TOTAL_DOCS (NUM_PRODUCERS * DOCS_PER_PRODUCER)

typedef struct
{
    int doc_id;
    char filename[60];
    int pages;
} Document;

Document queue[QUEUE_SIZE];

int head = 0;
int tail = 0;
int count = 0;

int all_sent = 0;

int submitted_docs = 0;
int printed_docs = 0;
int total_pages_printed = 0;

pthread_mutex_t q_lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

Document producer_docs[NUM_PRODUCERS][DOCS_PER_PRODUCER] =
{
    {
        {1, "report_Q1.pdf", 12},
        {2, "slides.pdf", 20},
        {3, "summary.pdf", 4}
    },
    {
        {4, "contract.pdf", 5},
        {5, "memo.pdf", 2},
        {6, "budget.pdf", 7}
    },
    {
        {7, "invoice.pdf", 3},
        {8, "proposal.pdf", 8},
        {9, "manual.pdf", 5}
    }
};

void enqueue(Document doc)
{
    queue[tail] = doc;
    tail = (tail + 1) % QUEUE_SIZE;
    count++;
}

Document dequeue_doc()
{
    Document doc = queue[head];
    head = (head + 1) % QUEUE_SIZE;
    count--;

    return doc;
}

void *producer(void *arg)
{
    int producer_id = *(int *)arg;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++)
    {
        Document doc = producer_docs[producer_id - 1][i];

        pthread_mutex_lock(&q_lock);

        while (count == QUEUE_SIZE)
        {
            printf("[Producer %d] Queue full - waiting...\n",
                   producer_id);

            pthread_cond_wait(&not_full, &q_lock);
        }

        enqueue(doc);
        submitted_docs++;

        printf("[Producer %d] Submitting: %-15s (%2d pages) - queue: %d/%d\n",
               producer_id,
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);

        usleep(200000);
    }

    return NULL;
}

void *printer(void *arg)
{
    (void)arg;

    while (1)
    {
        pthread_mutex_lock(&q_lock);

        while (count == 0 && !all_sent)
        {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        if (count == 0 && all_sent)
        {
            pthread_mutex_unlock(&q_lock);
            break;
        }

        Document doc = dequeue_doc();

        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&q_lock);

        printf("[Printer]    Printing:  %-15s (%2d pages) - queue: %d/%d\n",
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        printed_docs++;
        total_pages_printed += doc.pages;

        sleep(1);
    }

    printf("\n[Printer] All documents printed. Exiting.\n");

    return NULL;
}

int main()
{
    pthread_t producers[NUM_PRODUCERS];
    pthread_t printer_thread;

    int producer_ids[NUM_PRODUCERS] = {1, 2, 3};

    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    pthread_create(&printer_thread,
                   NULL,
                   printer,
                   NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        pthread_create(&producers[i],
                       NULL,
                       producer,
                       &producer_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        pthread_join(producers[i], NULL);
    }

    pthread_mutex_lock(&q_lock);

    all_sent = 1;

    pthread_cond_broadcast(&not_empty);

    pthread_mutex_unlock(&q_lock);

    pthread_join(printer_thread, NULL);

    printf("\n================ SUMMARY ================\n");
    printf("Documents submitted : %d\n", submitted_docs);
    printf("Documents printed   : %d\n", printed_docs);
    printf("Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}

/*
WHY pthread_cond_wait() MUST BE INSIDE A WHILE LOOP?

Wrong:

    if (count == 0)
        pthread_cond_wait(...);

Correct:

    while (count == 0)
        pthread_cond_wait(...);

Reason:
1. Another thread may consume/modify the queue before this thread
   acquires the mutex again.

2. POSIX allows spurious wakeups:
   pthread_cond_wait() may wake up even though no thread signaled it.

Therefore the condition must always be rechecked after wakeup.
*/