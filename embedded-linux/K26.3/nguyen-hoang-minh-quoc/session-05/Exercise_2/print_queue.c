/**
 * @file print_queue.c
 * @brief Office Print Queue simulation using Mutex and Condition Variables.
 *
 * ============================================================================
 * EXPLANATION: WHY pthread_cond_wait() MUST BE INSIDE A while LOOP
 * AND WHAT IS A SPURIOUS WAKEUP
 * ============================================================================
 * 1. Spurious Wakeup:
 *    A spurious wakeup is an event where a thread waiting on a condition
 *    variable via `pthread_cond_wait()` awakens even though NO explicit
 *    signal (`pthread_cond_signal` or `pthread_cond_broadcast`) was issued.
 *    This can happen due to internal kernel implementation details (such as
 *    Linux futex syscall handling, signal interruptions, context switches,
 *    or multiprocessor hardware race conditions).
 *
 * 2. Why a `while` loop is mandatory instead of an `if`:
 *    - Re-evaluating the Predicate: Even if the thread is awakened by a valid
 *      signal, another thread might have been scheduled first, acquired the
 *      mutex, and altered the shared state (e.g., consumed the only available
 *      slot in a bounded buffer, or dequeued the only document).
 *    - In the case of `pthread_cond_broadcast()`, multiple waiting threads
 *      wake up concurrently, but only one might be able to process the state
 *      change. The others must go back to sleep.
 *    - If an `if` statement were used, a thread returning from
 *      `pthread_cond_wait()` would immediately execute the critical section
 *      without checking if the condition actually holds. This would result in
 *      critical errors such as buffer overflow (writing when full) or buffer
 *      underflow (reading from an empty queue / segmentation fault).
 *    - Enclosing `pthread_cond_wait()` in a `while` loop guarantees that the
 *      condition (the predicate) is continually re-checked, ensuring the
 *      thread only proceeds when the condition is genuinely true.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define QUEUE_CAPACITY 5
#define NUM_PRODUCERS 3
#define DOCS_PER_PRODUCER 3
#define TOTAL_DOCS (NUM_PRODUCERS * DOCS_PER_PRODUCER)

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

/* Shared bounded queue and control variables */
Document queue[QUEUE_CAPACITY];
int head = 0, tail = 0, count = 0;
int all_sent = 0; /* set to 1 by main after joining all producers */

pthread_mutex_t q_lock;
pthread_cond_t  not_full;  /* producers wait here when count == QUEUE_CAPACITY */
pthread_cond_t  not_empty; /* printer waits here when count == 0 */

/* Statistics */
int docs_submitted = 0;
int docs_printed = 0;
int total_pages_printed = 0;

/* Producer configuration structure */
typedef struct {
    int producer_id;
    Document docs[DOCS_PER_PRODUCER];
} ProducerData;

/**
 * @brief Producer thread routine.
 *
 * Submits documents into the shared bounded queue.
 */
void *producer_func(void *arg)
{
    ProducerData *data = (ProducerData *)arg;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++) {
        pthread_mutex_lock(&q_lock);

        /* Wait while queue is full */
        while (count == QUEUE_CAPACITY) {
            printf("[Producer %d] Queue full — waiting...\n", data->producer_id);
            fflush(stdout);
            pthread_cond_wait(&not_full, &q_lock);
        }

        /* Enqueue document */
        queue[tail] = data->docs[i];
        tail = (tail + 1) % QUEUE_CAPACITY;
        count++;
        docs_submitted++;

        printf("[Producer %d] Submitting: %-14s (%2d pages) — queue: %d/%d\n",
               data->producer_id,
               data->docs[i].filename,
               data->docs[i].pages,
               count,
               QUEUE_CAPACITY);
        fflush(stdout);

        /* Notify printer that a document is available */
        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);

        /* Small delay to allow interleaved submissions */
        usleep(150000);
    }

    return NULL;
}

/**
 * @brief Printer thread routine.
 *
 * Pulls documents from the queue and prints them one by one.
 */
void *printer_func(void *arg)
{
    (void)arg;

    while (1) {
        Document current_doc;

        pthread_mutex_lock(&q_lock);

        /* Wait while queue is empty and producers are still sending */
        while (count == 0 && !all_sent) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        /* All documents have been submitted and queue is empty -> termination */
        if (count == 0 && all_sent) {
            pthread_mutex_unlock(&q_lock);
            break;
        }

        /* Dequeue document */
        current_doc = queue[head];
        head = (head + 1) % QUEUE_CAPACITY;
        count--;
        docs_printed++;
        total_pages_printed += current_doc.pages;

        printf("[Printer]    Printing:   %-14s (%2d pages) — queue: %d/%d\n",
               current_doc.filename,
               current_doc.pages,
               count,
               QUEUE_CAPACITY);
        fflush(stdout);

        /* Notify waiting producers that a slot has freed up */
        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&q_lock);

        /* Simulate printing time */
        sleep(1);
    }

    printf("[Printer]    All documents printed. Exiting.\n");
    fflush(stdout);

    return NULL;
}

int main(void)
{
    pthread_t prod_threads[NUM_PRODUCERS];
    pthread_t print_thread;

    /* Prepare sample documents matching expected test set (total 66 pages) */
    ProducerData producers[NUM_PRODUCERS] = {
        {
            .producer_id = 1,
            .docs = {
                {101, "report_Q1.pdf", 12},
                {102, "slides.pdf",    20},
                {103, "summary.pdf",    4}
            }
        },
        {
            .producer_id = 2,
            .docs = {
                {201, "contract.pdf",   5},
                {202, "memo.pdf",       2},
                {203, "budget.pdf",     7}
            }
        },
        {
            .producer_id = 3,
            .docs = {
                {301, "invoice.pdf",    3},
                {302, "proposal.pdf",   8},
                {303, "specs.pdf",      5}
            }
        }
    };

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: %d documents\n", QUEUE_CAPACITY);
    printf("==============================================\n\n");
    fflush(stdout);

    /* Initialize synchronization primitives */
    if (pthread_mutex_init(&q_lock, NULL) != 0 ||
        pthread_cond_init(&not_full, NULL) != 0 ||
        pthread_cond_init(&not_empty, NULL) != 0) {
        perror("Failed to initialize mutex or condition variables");
        return EXIT_FAILURE;
    }

    /* Start the printer thread */
    if (pthread_create(&print_thread, NULL, printer_func, NULL) != 0) {
        perror("Failed to create printer thread");
        return EXIT_FAILURE;
    }

    /* Start the 3 producer threads */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        if (pthread_create(&prod_threads[i], NULL, producer_func, &producers[i]) != 0) {
            perror("Failed to create producer thread");
            return EXIT_FAILURE;
        }
    }

    /* Wait for all producers to finish submitting */
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prod_threads[i], NULL);
    }

    /* Signal that all documents have been sent */
    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&q_lock);

    /* Wait for the printer to finish printing all queued documents */
    pthread_join(print_thread, NULL);

    /* Print final summary */
    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", docs_submitted);
    printf("  Documents printed   : %d\n", docs_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    /* Cleanup resources */
    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return EXIT_SUCCESS;
}
