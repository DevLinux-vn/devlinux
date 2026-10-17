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

/* Queue */
Document queue[QUEUE_SIZE];
int head = 0;
int tail = 0;
int count = 0;

/* Producer finished flag */
int all_sent = 0;

/* Statistics */
int submitted_docs = 0;
int printed_docs = 0;
int total_pages_printed = 0;

/* Synchronization */
pthread_mutex_t q_lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

/* Documents of 3 producers */
Document producer_docs[NUM_PRODUCERS][DOCS_PER_PRODUCER] =
{
    {
        {1, "report_Q1.pdf", 12},
        {2, "contract.pdf", 5},
        {3, "invoice.pdf", 3}
    },
    {
        {4, "slides.pdf", 20},
        {5, "memo.pdf", 2},
        {6, "proposal.pdf", 8}
    },
    {
        {7, "summary.pdf", 4},
        {8, "budget.pdf", 7},
        {9, "schedule.pdf", 5}
    }
};

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
    int producer_id = *(int *)arg;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++)
    {
        Document doc = producer_docs[producer_id][i];

        pthread_mutex_lock(&q_lock);

        while (count == QUEUE_SIZE)
        {
            pthread_cond_wait(&not_full, &q_lock);
        }

        enqueue(doc);
        submitted_docs++;

        printf("[Producer %d] Submitting: %-12s (%2d pages) - queue: %d/%d\n",
               producer_id + 1,
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);

        usleep(150000);
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

        printf("[Printer ] Printing: %-12s (%2d pages) - queue: %d/%d\n",
               doc.filename,
               doc.pages,
               count,
               QUEUE_SIZE);

        pthread_mutex_unlock(&q_lock);

        sleep(1);

        printed_docs++;
        total_pages_printed += doc.pages;
    }

    printf("[Printer ] All documents printed. Exiting.\n");

    return NULL;
}

int main()
{
    pthread_t producers[NUM_PRODUCERS];
    pthread_t printer_thread;

    int ids[NUM_PRODUCERS];

    printf("========================================\n");
    printf(" OFFICE PRINT QUEUE (3 producers)\n");
    printf(" Queue capacity: %d documents\n", QUEUE_SIZE);
    printf("========================================\n\n");

    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&printer_thread,
                   NULL,
                   printer,
                   NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {
        ids[i] = i;

        pthread_create(&producers[i],
                       NULL,
                       producer,
                       &ids[i]);
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

    printf("\n============= SUMMARY =============\n");
    printf("Documents submitted : %d\n", submitted_docs);
    printf("Documents printed   : %d\n", printed_docs);
    printf("Total pages printed : %d\n", total_pages_printed);
    printf("===================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}