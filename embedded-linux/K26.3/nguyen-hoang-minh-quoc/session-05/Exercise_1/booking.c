/**
 * @file booking.c
 * @brief Bus Ticket Booking System simulation using POSIX threads and mutex.
 *
 * ============================================================================
 * EXPLANATION: WHY CHECK AND DEDUCT MUST BE IN THE SAME CRITICAL SECTION
 * ============================================================================
 * In concurrent programming, checking the remaining seat count and deducting
 * the seats constitutes a compound operation (Check-Then-Act).
 *
 * If we split this into two separate lock acquisitions:
 *   1. Lock -> Check (if seats_available >= wanted) -> Unlock
 *   2. [Window of Vulnerability / Race Condition]
 *   3. Lock -> Deduct (seats_available -= wanted) -> Unlock
 *
 * A severe Time-Of-Check to Time-Of-Use (TOCTOU) race condition occurs:
 * - Thread A checks that 3 seats are available (satisfying its request for 2).
 *   Thread A then releases the lock before deducting.
 * - Thread B acquires the lock, checks that 3 seats are available, deducts 2,
 *   and releases the lock, leaving only 1 seat available.
 * - Thread A re-acquires the lock to perform the deduction, erroneously assuming
 *   there are still 3 seats available. Thread A deducts 2 seats from 1,
 *   causing seats_available to become -1 (overselling / negative inventory).
 *
 * Even if Thread A re-checks inside the second lock, splitting the locks creates
 * inconsistency where a confirmed reservation might suddenly fail, or threads
 * might see stale state.
 *
 * By enclosing BOTH the condition check and the deduction inside a SINGLE
 * atomic critical section (one lock/unlock pair), we ensure that no other
 * thread can read or modify `seats_available` between the moment we decide
 * there are enough seats and the moment the seats are reserved.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define TOTAL_SEATS 10
#define NUM_REQUESTS 5

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

/* Shared resources */
int seats_available = TOTAL_SEATS;
int seats_sold = 0;
int failed_bookings = 0;
pthread_mutex_t seat_lock;

/**
 * @brief Thread routine for each booking agent.
 *
 * Simulates concurrent ticket reservations with mutex protection.
 */
void *book_ticket(void *arg)
{
    BookingRequest *req = (BookingRequest *)arg;
    const char *initial_unit = (req->seats_wanted == 1) ? "seat " : "seats";

    /* Display agent start with TID to demonstrate concurrency */
    printf("[Agent %d | TID %lu] Booking %d %s for %s...\n",
           req->agent_id,
           (unsigned long)pthread_self(),
           req->seats_wanted,
           initial_unit,
           req->customer);
    fflush(stdout);

    /* Force real concurrency and simulate network/processing delay */
    sleep(1);

    /* ATOMIC CRITICAL SECTION: Check and deduct under the same lock */
    pthread_mutex_lock(&seat_lock);

    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;
        seats_sold += req->seats_wanted;
        const char *seat_unit = (req->seats_wanted == 1) ? "seat " : "seats";
        printf("[Agent %d] CONFIRMED: %d %s for %s.  Remaining: %d\n",
               req->agent_id,
               req->seats_wanted,
               seat_unit,
               req->customer,
               seats_available);
    } else {
        failed_bookings++;
        const char *seat_unit = (req->seats_wanted == 1) ? "seat " : "seats";
        printf("[Agent %d] SOLD OUT:  needs %d %s, only %d left — booking failed.\n",
               req->agent_id,
               req->seats_wanted,
               seat_unit,
               seats_available);
    }

    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

int main(void)
{
    /* Hardcoded array of 5 requests as specified in problem statement */
    BookingRequest requests[NUM_REQUESTS] = {
        {1, "Nguyen Van An",  2},
        {2, "Tran Thi Bich",  1},
        {3, "Le Van Cuong",   3},
        {4, "Pham Thi Dung",  1},
        {5, "Hoang Van Em",   2}
    };

    pthread_t threads[NUM_REQUESTS];

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");
    fflush(stdout);

    /* Initialize the mutex */
    if (pthread_mutex_init(&seat_lock, NULL) != 0) {
        perror("Failed to initialize mutex");
        return EXIT_FAILURE;
    }

    /* Create 5 agent threads */
    for (int i = 0; i < NUM_REQUESTS; i++) {
        if (pthread_create(&threads[i], NULL, book_ticket, &requests[i]) != 0) {
            perror("Failed to create agent thread");
            pthread_mutex_destroy(&seat_lock);
            return EXIT_FAILURE;
        }
    }

    /* Wait for all 5 agent threads to finish */
    for (int i = 0; i < NUM_REQUESTS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Print final summary */
    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", TOTAL_SEATS);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    /* Destroy mutex before exiting */
    pthread_mutex_destroy(&seat_lock);

    return EXIT_SUCCESS;
}
