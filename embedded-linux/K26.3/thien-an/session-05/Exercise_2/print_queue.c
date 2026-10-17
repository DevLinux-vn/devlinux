/*
 * THEORY EXPLANATION:
 * 1. Why use a 'while' loop instead of an 'if' for pthread_cond_wait()?
 *    When a thread wakes up from pthread_cond_wait(), there is NO guarantee 
 *    that the condition it was waiting for is still true. Another fast thread 
 *    might have snatched the lock first and changed the shared state (e.g., 
 *    made the queue full again). The 'while' loop forces the thread to re-check 
 *    the condition after waking up. If the condition is still not met, it goes 
 *    back to sleep safely.
 *
 * 2. What is a "Spurious wakeup"?
 *    A spurious wakeup is a low-level OS phenomenon where a waiting thread is 
 *    woken up without any explicit pthread_cond_signal() or broadcast() being 
 *    called. It happens due to how POSIX condition variables are implemented 
 *    under the hood (often related to OS signal interruptions). Wrapping the 
 *    wait call inside a 'while' loop is the standard defensive programming 
 *    technique to catch and ignore these false alarms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define QUEUE_CAPACITY 5

// Data structures
typedef struct {
	int  doc_id;
	char filename[60];
	int  pages;
} Document;

typedef struct {
	int producer_id;
	Document docs[3];
} ProducerData;

// Global shared resources[cite: 4]
Document queue[QUEUE_CAPACITY];
int head = 0, tail = 0, count = 0;
int all_sent = 0; /* set to 1 by main after joining all producers */

// Synchronization tools[cite: 4]
pthread_mutex_t q_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;

// Stats tracking
int total_submitted = 0;
int total_printed = 0;
int total_pages = 0;

void *producer(void *arg) {
	ProducerData *p_data = (ProducerData *)arg;
	
	// Each producer submits exactly 3 documents[cite: 4]
	for (int i = 0; i < 3; i++) {
		pthread_mutex_lock(&q_lock);
		
		// Wait if the queue is full (must use while loop)[cite: 4]
		while (count == QUEUE_CAPACITY) {
			printf("[Producer %d] Queue full — waiting...\n", p_data->producer_id);
			pthread_cond_wait(&not_full, &q_lock);
		}
		
		// Enqueue document[cite: 4]
		queue[tail] = p_data->docs[i];
		tail = (tail + 1) % QUEUE_CAPACITY;
		count++;
		total_submitted++;
		
		printf("[Producer %d] Submitting: %-13s (%2d pages) — queue: %d/%d\n", 
		       p_data->producer_id, p_data->docs[i].filename, p_data->docs[i].pages, count, QUEUE_CAPACITY);
		
		// Wake up the printer if it was sleeping[cite: 4]
		pthread_cond_signal(&not_empty);
		pthread_mutex_unlock(&q_lock);
		
		// Small sleep to interleave producer outputs natively
		usleep(300000);
	}
	
	return NULL;
}

void *printer(void *arg) {
	(void)arg; // Unused
	
	while (1) {
		pthread_mutex_lock(&q_lock);
		
		// Wait if the queue is empty AND producers are still active[cite: 4]
		while (count == 0 && !all_sent) {
			pthread_cond_wait(&not_empty, &q_lock);
		}
		
		// Exit condition: queue is empty and no more documents will come[cite: 4]
		if (count == 0 && all_sent) {
			pthread_mutex_unlock(&q_lock);
			break;
		}
		
		// Dequeue document[cite: 4]
		Document doc = queue[head];
		head = (head + 1) % QUEUE_CAPACITY;
		count--;
		total_printed++;
		total_pages += doc.pages;
		
		// Wake up a producer that might be waiting on a full queue[cite: 4]
		pthread_cond_signal(&not_full);
		pthread_mutex_unlock(&q_lock);
		
		printf("[Printer]    Printing:   %-13s (%2d pages) — queue: %d/%d\n", 
		       doc.filename, doc.pages, count, QUEUE_CAPACITY);
		
		// Simulate printing time[cite: 4]
		sleep(1); 
	}
	
	printf("[Printer]    All documents printed. Exiting.\n");
	return NULL;
}

int main() {
	// Prepare hardcoded data to match the total 66 pages from expected output
	ProducerData p_data[3] = {
		{1, {{1, "report_Q1.pdf", 12}, {4, "slides.pdf", 20}, {7, "summary.pdf", 4}}},
		{2, {{2, "contract.pdf", 5}, {5, "memo.pdf", 2}, {8, "budget.pdf", 7}}},
		{3, {{3, "invoice.pdf", 3}, {6, "proposal.pdf", 8}, {9, "appendix.pdf", 5}}}
	};
	
	pthread_t prods[3];
	pthread_t print_th;
	
	printf("==============================================\n");
	printf("   OFFICE PRINT QUEUE (3 producers, 1 printer)\n");
	printf("   Queue capacity: 5 documents\n");
	printf("==============================================\n\n");
	
	// Create printer thread[cite: 4]
	if (pthread_create(&print_th, NULL, printer, NULL) != 0) {
		perror("Failed to create printer thread");
		return 1;
	}
	
	// Create 3 producer threads[cite: 4]
	for (int i = 0; i < 3; i++) {
		if (pthread_create(&prods[i], NULL, producer, &p_data[i]) != 0) {
			perror("Failed to create producer thread");
			return 1;
		}
	}
	
	// Join all 3 producers[cite: 4]
	for (int i = 0; i < 3; i++) {
		pthread_join(prods[i], NULL);
	}
	
	// Signal printer that all producers are done[cite: 4]
	pthread_mutex_lock(&q_lock);
	all_sent = 1;
	pthread_cond_broadcast(&not_empty); // Wake printer if it is sleeping on empty queue[cite: 4]
	pthread_mutex_unlock(&q_lock);
	
	// Wait for printer to finish printing remaining documents[cite: 4]
	pthread_join(print_th, NULL);
	
	// Cleanup synchronization primitives
	pthread_mutex_destroy(&q_lock);
	pthread_cond_destroy(&not_empty);
	pthread_cond_destroy(&not_full);
	
	// Print Summary table[cite: 4]
	printf("\n================ SUMMARY ================\n");
	printf("  Documents submitted : %d\n", total_submitted);
	printf("  Documents printed   : %d\n", total_printed);
	printf("  Total pages printed : %d\n", total_pages);
	printf("=========================================\n");
	
	return 0;
}