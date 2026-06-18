/*
 * print_queue.c — Office print queue: 3 producers, 1 printer, capacity 5.
 *
 * WHY pthread_cond_wait() must be inside a WHILE loop, not an IF:
 *
 * A "spurious wakeup" is when a thread is woken from pthread_cond_wait()
 * without any other thread having called pthread_cond_signal/broadcast on
 * that condition variable. The POSIX standard explicitly permits this — it
 * can happen due to OS scheduler internals or signal delivery.
 *
 * If we use `if (count == 0) pthread_cond_wait(...)`, a spuriously woken
 * thread resumes without the condition actually being true, so it proceeds
 * to dequeue from an empty queue — undefined behaviour (reading stale data).
 *
 * Using `while (count == 0 && !all_sent) pthread_cond_wait(...)` ensures
 * the condition is re-checked every time the thread wakes up, whether the
 * wakeup was real or spurious. Only when the condition is genuinely satisfied
 * does the thread proceed.
 *
 * The same logic applies to producers: `while (count == 5)` guards against
 * spurious wakeups that would let a producer enqueue into a full queue.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_CAP   5
#define NUM_PROD    3
#define DOCS_EACH   3   /* each producer sends exactly 3 documents */

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

/* Circular queue */
Document queue[QUEUE_CAP];
int head = 0, tail = 0, count = 0;

int all_sent = 0; /* set to 1 by main after all producers finish */

pthread_mutex_t q_lock;
pthread_cond_t  not_full;   /* producers wait here when count == QUEUE_CAP */
pthread_cond_t  not_empty;  /* printer  waits here when count == 0         */

/* Global counters (only written by producer/printer while holding q_lock) */
int total_submitted = 0;
int total_printed   = 0;
int total_pages     = 0;

/* Document catalogue — 9 docs spread across 3 producers */
static const Document catalogue[NUM_PROD][DOCS_EACH] = {
    {
        {1, "report_Q1.pdf", 12},
        {4, "slides.pdf",    20},
        {7, "summary.pdf",    4},
    },
    {
        {2, "contract.pdf",   5},
        {5, "memo.pdf",       2},
        {8, "budget.pdf",     7},
    },
    {
        {3, "invoice.pdf",    3},
        {6, "proposal.pdf",   8},
        {9, "letter.pdf",     1},
    },
};

static void enqueue(const Document *doc)
{
    queue[tail] = *doc;
    tail = (tail + 1) % QUEUE_CAP;
    count++;
}

static Document dequeue(void)
{
    Document doc = queue[head];
    head = (head + 1) % QUEUE_CAP;
    count--;
    return doc;
}

static void *producer(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < DOCS_EACH; i++) {
        const Document *doc = &catalogue[id - 1][i];

        pthread_mutex_lock(&q_lock);

        while (count == QUEUE_CAP) {
            printf("[Producer %d] Queue full — waiting...\n", id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        enqueue(doc);
        total_submitted++;
        printf("[Producer %d] Submitting: %-20s (%2d pages) — queue: %d/%d\n",
               id, doc->filename, doc->pages, count, QUEUE_CAP);

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&q_lock);

        /* small delay so producers don't all race in a tight burst */
        usleep(100000); /* 100 ms */
    }

    return NULL;
}

static void *printer(void *arg)
{
    (void)arg;

    for (;;) {
        pthread_mutex_lock(&q_lock);

        while (count == 0 && !all_sent)
            pthread_cond_wait(&not_empty, &q_lock);

        if (count == 0 && all_sent) {
            pthread_mutex_unlock(&q_lock);
            break;
        }

        Document doc = dequeue();
        total_printed++;
        total_pages += doc.pages;
        printf("[Printer]    Printing:   %-20s (%2d pages) — queue: %d/%d\n",
               doc.filename, doc.pages, count, QUEUE_CAP);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&q_lock);

        sleep(1); /* simulate printing time */
    }

    printf("[Printer]    All documents printed. Exiting.\n");
    return NULL;
}

int main(void)
{
    pthread_t prod_threads[NUM_PROD];
    pthread_t print_thread;
    int ids[NUM_PROD] = {1, 2, 3};

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: %d documents\n", QUEUE_CAP);
    printf("==============================================\n\n");

    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full,  NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&print_thread, NULL, printer, NULL);

    for (int i = 0; i < NUM_PROD; i++)
        pthread_create(&prod_threads[i], NULL, producer, &ids[i]);

    for (int i = 0; i < NUM_PROD; i++)
        pthread_join(prod_threads[i], NULL);

    /* Signal printer that no more documents will arrive */
    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&q_lock);

    pthread_join(print_thread, NULL);

    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    pthread_mutex_destroy(&q_lock);

    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", total_submitted);
    printf("  Documents printed   : %d\n", total_printed);
    printf("  Total pages printed : %d\n", total_pages);
    printf("=========================================\n");

    return 0;
}
