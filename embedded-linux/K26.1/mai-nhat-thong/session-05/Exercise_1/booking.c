#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/* Booking Request Structure */
typedef struct {
    int agent_id;
    char customer[50];
    int seats_wanted;
} BookingRequest;

/* Shared Resources */
int seats_available = 10;
int seats_sold = 0;
int failed_bookings = 0;

/* Synchronization Primitives */
pthread_mutex_t seat_lock;

/**
 * @brief Thread routine representing a ticket booking agent.
 * @param arg Pointer to the BookingRequest structure.
 */
void* book_ticket(void* arg) {
    BookingRequest* req = (BookingRequest*)arg;
    
    /* Display the initial reservation request along with unique Thread ID (TID) */
    printf("[Agent %d | TID %lu] Booking %d %s for %s...\n", 
           req->agent_id, 
           (unsigned long)pthread_self(), 
           req->seats_wanted, 
           req->seats_wanted > 1 ? "seats" : "seat", 
           req->customer);
    
    /* Force real concurrency by putting threads to sleep simultaneously.
       This forces all threads to wake up around the same time and contentiously 
       race for the critical section. */
    sleep(1);

    /* ---------------------------------------------------------------------- */
    /* CRITICAL SECTION BEGIN                                                 */
    /* ---------------------------------------------------------------------- */
    pthread_mutex_lock(&seat_lock);

    /* Atomic Check-and-Deduct operation protected by the mutex */
    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;
        seats_sold += req->seats_wanted;
        printf("[Agent %d] CONFIRMED: %d %s for %s. Remaining: %d\n", 
               req->agent_id, 
               req->seats_wanted, 
               req->seats_wanted > 1 ? "seats" : "seat", 
               req->customer, 
               seats_available);
    } else {
        printf("[Agent %d] SOLD OUT: needs %d %s, only %d left — booking failed.\n", 
               req->agent_id, 
               req->seats_wanted, 
               req->seats_wanted > 1 ? "seats" : "seat", 
               seats_available);
        failed_bookings++;
    }

    pthread_mutex_unlock(&seat_lock);
    /* ---------------------------------------------------------------------- */
    /* CRITICAL SECTION END                                                   */
    /* ---------------------------------------------------------------------- */

    return NULL;
}

/* ==========================================================================
 * SENIOR ARCHITECT EXPLANATION: WHY CHECK-AND-DEDUCT MUST BE IN ONE BLOCK
 * ==========================================================================
 * Splitting the check (if seats_available >= wanted) and the deduction 
 * (seats_available -= wanted) into two separate lock acquisitions introduces 
 * a dangerous "Race Condition" known as a Time-of-Check to Time-of-Use (TOCTOU) bug.
 * * If split:
 * 1. Agent A acquires the lock, checks that 3 seats are left, and unlocks.
 * 2. Context switch occurs. Agent B acquires the lock, checks that 3 seats 
 * are left, deducts 2 seats, and unlocks (Remaining: 1).
 * 3. Context switch back to Agent A. Agent A re-acquires the lock and blindly 
 * deducts 3 seats based on its *stale* check data.
 * 4. Result: seats_available becomes 1 - 3 = -2 (Overbooked / Data Corruption).
 * * Therefore, both checking and deducting must be executed as a single, 
 * indivisible, atomic transaction inside the exact same lock/unlock block.
 * ========================================================================== */

int main(void) {
    /* Hardcoded array of 5 requests as required by the specification */
    BookingRequest requests[5] = {
        { 1, "Nguyen Van An",   2 },
        { 2, "Tran Thi Bich",   1 },
        { 3, "Le Van Cuong",    3 },
        { 4, "Pham Thi Dung",   1 },
        { 5, "Hoang Van Em",    2 }
    };

    pthread_t threads[5];

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    /* Initialize the mutex with default attributes */
    if (pthread_mutex_init(&seat_lock, NULL) != 0) {
        perror("Mutex initialization failed");
        return EXIT_FAILURE;
    }

    /* Spawn 5 concurrent agent threads */
    for (int i = 0; i < 5; i++) {
        if (pthread_create(&threads[i], NULL, book_ticket, &requests[i]) != 0) {
            perror("Failed to create thread");
            return EXIT_FAILURE;
        }
    }

    /* Synchronize and wait for all agents to complete their executions */
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Destroy the mutex to free up operating system resources */
    pthread_mutex_destroy(&seat_lock);

    /* Print out the exact required summary report */
    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : 10\n");
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    return EXIT_SUCCESS;
}