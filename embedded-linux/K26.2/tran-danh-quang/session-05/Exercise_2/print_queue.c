#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_QUEUE_SIZE 5
#define PRODUCER_COUNT 3
#define DOCS_PER_PRODUCER 3

typedef struct {
    int  doc_id;
    char filename[60];
    int  pages;
} Document;

Document queue[MAX_QUEUE_SIZE];
int head = 0, tail = 0, count = 0;
int all_sent = 0;
int docs_submitted = 0;
int docs_printed = 0;
int total_pages_printed = 0;

pthread_mutex_t q_lock;
pthread_cond_t  not_full;
pthread_cond_t  not_empty;

Document docs_to_submit[PRODUCER_COUNT][DOCS_PER_PRODUCER] = {
    { {1, "report_Q1.pdf", 12}, {4, "slides.pdf", 20}, {7, "summary.pdf", 4} },
    { {2, "contract.pdf", 5},   {5, "memo.pdf", 2},    {8, "budget.pdf", 7} },
    { {3, "invoice.pdf", 3},    {6, "proposal.pdf", 8}, {9, "final.pdf", 1} }
};

void* producer(void* arg) {
    int p_id = *(int*)arg;
    for (int i = 0; i < DOCS_PER_PRODUCER; i++) {
        Document doc = docs_to_submit[p_id - 1][i];

        pthread_mutex_lock(&q_lock);

        /*
         * WHY pthread_cond_wait MUST BE IN A WHILE LOOP:
         * A thread waiting on a condition variable might be woken up without the condition 
         * actually being met (a "spurious wakeup" caused by the OS). If we used an 'if' 
         * statement, the thread would proceed and overflow the queue. By using a 'while' 
         * loop, the thread re-checks the condition after waking up. If the condition is 
         * still not met (e.g. queue is still full), it goes back to sleep.
         * 
         * SPURIOUS WAKEUP: An event where a thread wakes up from waiting on a condition 
         * variable without receiving a signal, or when another thread has already changed 
         * the condition before this thread acquired the mutex.
         */
        while (count == MAX_QUEUE_SIZE) {
            printf("[Producer %d] Queue full — waiting...\n", p_id);
            pthread_cond_wait(&not_full, &q_lock);
        }

        // enqueue
        queue[tail] = doc;
        tail = (tail + 1) % MAX_QUEUE_SIZE;
        count++;
        docs_submitted++;

        /*
         * NOTE (Performance Anti-pattern):
         * Placing printf() inside a critical section is kept here for assignment purposes 
         * to prevent interleaved console logs. In high-performance systems, I/O should 
         * be kept outside of mutexes to prevent blocking and thread contention.
         */
        printf("[Producer %d] Submitting: %-13s (%2d pages) — queue: %d/%d\n",
               p_id, doc.filename, doc.pages, count, MAX_QUEUE_SIZE);

        pthread_cond_signal(&not_empty); // wake up printer if it was sleeping
        pthread_mutex_unlock(&q_lock);

        usleep(300000); // slight delay between submissions for better interleaving
    }
    return NULL;
}

void* printer(void* arg) {
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

        // dequeue
        Document doc = queue[head];
        head = (head + 1) % MAX_QUEUE_SIZE;
        count--;
        
        /*
         * NOTE (Performance Anti-pattern):
         * Same as above: I/O inside mutex is for preventing interleaved logs during the 
         * simulation. In real production, we'd copy the state and print outside the lock.
         */
        printf("[Printer]    Printing:   %-13s (%2d pages) — queue: %d/%d\n",
               doc.filename, doc.pages, count, MAX_QUEUE_SIZE);
               
        docs_printed++;
        total_pages_printed += doc.pages;

        pthread_cond_signal(&not_full); // wake up any waiting producer
        pthread_mutex_unlock(&q_lock);

        usleep(500000); // simulate printing time
    }
    printf("[Printer]    All documents printed. Exiting.\n");
    return NULL;
}

int main(void) {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t printer_thread;
    int p_ids[PRODUCER_COUNT] = {1, 2, 3};

    printf("==============================================\n");
    printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
    printf("   Queue capacity: 5 documents\n");
    printf("==============================================\n\n");

    if (pthread_mutex_init(&q_lock, NULL) != 0) {
        perror("Mutex init failed");
        return EXIT_FAILURE;
    }
    if (pthread_cond_init(&not_full, NULL) != 0) {
        perror("Cond init failed");
        return EXIT_FAILURE;
    }
    if (pthread_cond_init(&not_empty, NULL) != 0) {
        perror("Cond init failed");
        return EXIT_FAILURE;
    }

    if (pthread_create(&printer_thread, NULL, printer, NULL) != 0) {
        perror("Printer thread failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        if (pthread_create(&producers[i], NULL, producer, &p_ids[i]) != 0) {
            perror("Producer thread failed");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }

    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty); // wake up printer if it is stuck waiting for empty
    pthread_mutex_unlock(&q_lock);

    pthread_join(printer_thread, NULL);

    printf("\n================ SUMMARY ================\n");
    printf("  Documents submitted : %d\n", docs_submitted);
    printf("  Documents printed   : %d\n", docs_printed);
    printf("  Total pages printed : %d\n", total_pages_printed);
    printf("=========================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
