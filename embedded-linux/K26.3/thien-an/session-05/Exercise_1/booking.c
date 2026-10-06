#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
 * WHY CHECK AND DEDUCT MUST BE IN THE SAME LOCK/UNLOCK BLOCK:
 * If the check (e.g., seats >= wanted) and the deduct (seats -= wanted)
 * are split into two separate lock acquisitions, a critical race condition occurs.
 * For example, Agent 1 locks, checks that there are enough seats, and unlocks.
 * Before Agent 1 can re-acquire the lock to actually deduct the seats, Agent 2 
 * might acquire the lock, check, and deduct those exact same seats. When Agent 1 
 * finally proceeds to deduct, it will oversell the tickets, leading to negative 
 * available seats. Therefore, checking and deducting must be a single, indivisible 
 * (atomic) operation within the same critical section.
 */

typedef struct {
	int  agent_id;
	char customer[50];
	int  seats_wanted;
} BookingRequest;

// Global shared resources
int seats_available = 10;
int failed_bookings = 0;
pthread_mutex_t seat_lock;

void *book_ticket(void *arg) {
	BookingRequest *req = (BookingRequest *)arg;
	
	// Determine pluralization for cleaner output formatting
	const char *s_plural = (req->seats_wanted > 1) ? "seats" : "seat ";
	
	// Print intent using pthread_self() to observe interleaved scheduling[cite: 4]
	printf("[Agent %d | TID %lu] Booking %d %s for %s...\n", 
	       req->agent_id, (unsigned long)pthread_self(), req->seats_wanted, s_plural, req->customer);
	
	// Sleep to force all threads to overlap and compete for the mutex simultaneously[cite: 4]
	sleep(1);
	
	// Enter critical section[cite: 4]
	pthread_mutex_lock(&seat_lock);
	
	if (seats_available >= req->seats_wanted) {
		seats_available -= req->seats_wanted;
		printf("[Agent %d] CONFIRMED: %d %s for %s. Remaining: %d\n",
		       req->agent_id, req->seats_wanted, s_plural, req->customer, seats_available);
	} else {
		failed_bookings++;
		printf("[Agent %d] SOLD OUT:  needs %d %s, only %d left — booking failed.\n",
		       req->agent_id, req->seats_wanted, (req->seats_wanted > 1) ? "seats" : "seat ", seats_available);
	}
	
	// Exit critical section[cite: 4]
	pthread_mutex_unlock(&seat_lock);
	
	return NULL;
}

int main() {
	// Hardcoded array of 5 requests[cite: 4]
	BookingRequest requests[5] = {
		{1, "Nguyen Van An",  2},
		{2, "Tran Thi Bich",  1},
		{3, "Le Van Cuong",   3},
		{4, "Pham Thi Dung",  1},
		{5, "Hoang Van Em",   2}
	};
	
	pthread_t threads[5];
	int num_requests = 5;
	int total_seats_initial = seats_available;
	
	printf("==============================================\n");
	printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
	printf("==============================================\n");
	
	// Initialize the mutex[cite: 4]
	if (pthread_mutex_init(&seat_lock, NULL) != 0) {
		perror("Mutex initialization failed");
		return 1;
	}
	
	// Create 5 agent threads[cite: 4]
	for (int i = 0; i < num_requests; i++) {
		if (pthread_create(&threads[i], NULL, book_ticket, &requests[i]) != 0) {
			perror("Failed to create thread");
			return 1;
		}
	}
	
	// Join all 5 threads to wait for their completion[cite: 4]
	for (int i = 0; i < num_requests; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// Destroy the mutex after all threads are done[cite: 4]
	pthread_mutex_destroy(&seat_lock);
	
	// Print final summary[cite: 4]
	printf("\n================ SUMMARY ================\n");
	printf("  Total seats     : %d\n", total_seats_initial);
	printf("  Seats sold      : %d\n", total_seats_initial - seats_available);
	printf("  Seats remaining : %d\n", seats_available);
	printf("  Failed bookings : %d\n", failed_bookings);
	printf("=========================================\n");
	
	return 0;
}