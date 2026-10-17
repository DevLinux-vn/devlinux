#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* Document structure */
typedef struct {
    int doc_id;
    char filename[60];
    int pages;
} Document;

/* Shared Circular Buffer Queue configuration */
#define QUEUE_CAPACITY 5
Document queue[QUEUE_CAPACITY];
int head = 0;
int tail = 0;
int count = 0;

/* Execution Control State flags */
int all_sent = 0; 

/* Statistical Trackers */
int total_submitted = 0;
int total_printed = 0;
int total_pages_printed = 0;

/* Condition Variable Synchronization Tools */
pthread_mutex_t q_lock;
pthread_cond_t not_full;   /* Signaled when an item is removed (slot available) */
pthread_cond_t not_empty;  /* Signaled when an item is added (data available) */

/**
 * @brief Producer thread routine submitting documents to the shared queue.
 */
void* producer(void* arg) {
    int p_id = *(int*)arg;
    free(arg); /* Free the dynamically allocated thread argument */

    /* Each producer submits exactly 3 structural documents */
    for (int i = 1; i <= 3; i++) {
        Document doc;
        doc.doc_id = p_id * 10 + i; /* Create a unique identifier */
        doc.pages = (p_id * 5) + (i * 3 + 1); /* Generate deterministic sample page numbers */
        
        if (i == 1) sprintf(doc.filename, "report_Q%d.pdf", p_id);
        else if (i == 2) sprintf(doc.filename, "contract_v%d.pdf", p_id);
        else sprintf(doc.filename, "invoice_%d.pdf", doc.doc_id);

        pthread_mutex_lock(&q_lock);

        /* Guard the condition using a while loop to protect against Spurious Wakeups */
        while (count == QUEUE_CAPACITY) {
            printf("[Producer %d] Queue full — waiting...\n", p_id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        /* Enqueue the document into the circular buffer */
        queue[tail] = doc;
        tail = (tail + 1) % QUEUE_CAPACITY;
        count++;
        total_submitted++;

        printf("[Producer %d] Submitting: %s (%d pages) — queue: %d/%d\n", 
               p_id, doc.filename, doc.pages, count, QUEUE_CAPACITY);

        /* Signal the printer thread that a new document is ready */
        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);
        
        /* Slight sleep to distribute production bursts naturally */
        usleep(100000); 
    }
    return NULL;
}

/**
 * @brief Consumer thread routine extracting and printing documents from the queue.
 */
void* printer(void* arg) {
    (void)arg; /* Unused parameter */

    while (1) {
        Document doc_to_print;
        
        pthread_mutex_lock(&q_lock);

        /* The printer must sleep if the queue is empty AND producers are still active */
        while (count == 0 && !all_sent) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        /* Graceful termination condition: All producers joined and queue completely emptied */
        if (count == 0 && all_sent) {
            pthread_mutex_unlock(&q_lock);
            break;
        }

        /* Dequeue the document from the circular buffer */
        doc_to_print = queue[head];
        head = (head + 1) % QUEUE_CAPACITY;
        count--;

        printf("[Printer]    Printing:   %s (%d pages) — queue: %d/%d\n", 
               doc_to_print.filename, doc_to_print.pages, count, QUEUE_CAPACITY);

        /* Signal any waiting producers that a slot has freed up */
        pthread_cond_signal(&not_full);

        pthread_mutex_unlock(&q_lock);

        /* Simulate hardware mechanical printing overhead time outside the lock */
        sleep(1); 

        /* Record metrics securely outside the critical section to reduce lock contention */
        total_printed++;
        total_pages_printed += doc_to_print.pages;
    }

    printf("[Printer]    All documents printed. Exiting.\n");
    return NULL;
}

/* ==========================================================================
 * SENIOR ARCHITECT EXPLANATION: SPURIOUS WAKEUPS AND THE WHILE-LOOP MANDATE
 * ==========================================================================
 * A "Spurious Wakeup" happens when a thread waiting on a condition variable 
 * wakes up *without* any explicit signal or broadcast being sent by another thread.
 * This can be triggered by internal OS kernel optimizations, signal delivery, 
 * or thread scheduling variations.
 * * Why a 'while' loop is mandatory instead of an 'if':
 * - If you use an 'if' statement, a thread woken up spuriously will directly 
 * proceed to access the shared buffer without verifying if the predicate is true.
 * - For example: The printer wakes up spuriously when count == 0. If guarded 
 * by an 'if', it bypasses the block, attempts to read from queue[head], 
 * resulting in undefined behavior or data corruption.
 * * By using a 'while' loop, when the thread wakes up (either legitimately or 
 * spuriously), it is forced to re-evaluate the condition (`count == 0` or `count == 5`).
 * If the condition is still not met, it safely goes back to sleep.
 * ========================================================================== */

int main(void) {
    pthread_t producers[3];
    pthread_t printer_th;

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    /* Initialize sync primitives */
    pthread_mutex_init(&q_lock, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    /* Spawn the single printer consumer thread */
    if (pthread_create(&printer_th, NULL, printer, NULL) != 0) {
        perror("Failed to create printer thread");
        return EXIT_FAILURE;
    }

    /* Spawn 3 separate producer threads */
    for (int i = 0; i < 3; i++) {
        int* id_alloc = malloc(sizeof(int));
        *id_alloc = i + 1;
        if (pthread_create(&producers[i], NULL, producer, id_alloc) != 0) {
            perror("Failed to create producer thread");
            return EXIT_FAILURE;
        }
    }

    /* Wait for all 3 producers to finish compiling and submitting their work */
    for (int i = 0; i < 3; i++) {
        pthread_join(producers[i], NULL);
    }

    /* ---------------------------------------------------------------------- */
    /* STATE TRANSITION: Notify printer that no more documents will arrive    */
    /* ---------------------------------------------------------------------- */
    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    /* Broadcast to ensure that if the printer is sleeping on an empty queue, 
       it wakes up immediately to recognize `all_sent` and exit cleanly. */
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&q_lock);

    /* Wait for the printer to finish printing remaining buffered documents */
    pthread_join(printer_th, NULL);

    /* Free POSIX kernel resources */
    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    /* Output Final Summary Report */
    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", total_submitted);
    printf("  Documents printed   : %d\n", total_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    return EXIT_SUCCESS;
}