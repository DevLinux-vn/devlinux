/*
 * booking.c — Bus ticket booking system with 5 concurrent agents.
 *
 * WHY check and deduct must be inside the SAME lock/unlock block:
 *
 * If we split them into two separate critical sections:
 *
 *   pthread_mutex_lock(&seat_lock);
 *   int ok = (seats_available >= wanted);   // check
 *   pthread_mutex_unlock(&seat_lock);
 *   // <--- GAP: other threads can run here and change seats_available
 *   pthread_mutex_lock(&seat_lock);
 *   if (ok) seats_available -= wanted;      // deduct
 *   pthread_mutex_unlock(&seat_lock);
 *
 * Between the two locks, another thread can deduct seats so that
 * seats_available drops below `wanted`. But the first thread already
 * saw ok=1, so it proceeds to deduct anyway — resulting in
 * seats_available going negative (overbooking). This is a classic
 * TOCTOU (Time-Of-Check-Time-Of-Use) race condition.
 *
 * Keeping both operations inside ONE lock/unlock guarantees they are
 * atomic: no other thread can observe or modify the shared state
 * between the check and the deduct.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

BookingRequest requests[5] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",  1},
    {3, "Le Van Cuong",   3},
    {4, "Pham Thi Dung",  1},
    {5, "Hoang Van Em",   2}
};

int seats_available = 10;
pthread_mutex_t seat_lock;

/* Shared counters updated inside the critical section */
int seats_sold    = 0;
int failed_count  = 0;

static void *book_ticket(void *arg)
{
    BookingRequest *req = (BookingRequest *)arg;
    pthread_t tid = pthread_self();

    printf("[Agent %d | TID %lu] Booking %d seat%s for %s...\n",
           req->agent_id, (unsigned long)tid,
           req->seats_wanted,
           req->seats_wanted == 1 ? " " : "s",
           req->customer);

    sleep(1); /* force all agents to reach the mutex at roughly the same time */

    pthread_mutex_lock(&seat_lock);

    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;
        seats_sold      += req->seats_wanted;
        printf("[Agent %d] CONFIRMED: %d seat%s for %s. Remaining: %d\n",
               req->agent_id,
               req->seats_wanted,
               req->seats_wanted == 1 ? " " : "s",
               req->customer,
               seats_available);
    } else {
        failed_count++;
        printf("[Agent %d] SOLD OUT:  needs %d seat%s, only %d left — booking failed.\n",
               req->agent_id,
               req->seats_wanted,
               req->seats_wanted == 1 ? " " : "s",
               seats_available);
    }

    pthread_mutex_unlock(&seat_lock);
    return NULL;
}

int main(void)
{
    pthread_t threads[5];
    int total_seats = seats_available;

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    pthread_mutex_init(&seat_lock, NULL);

    for (int i = 0; i < 5; i++)
        pthread_create(&threads[i], NULL, book_ticket, &requests[i]);

    printf("\n--- [all agents reach critical section after sleep(1)] ---\n\n");

    for (int i = 0; i < 5; i++)
        pthread_join(threads[i], NULL);

    pthread_mutex_destroy(&seat_lock);

    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", total_seats);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_count);
    printf("=========================================\n");

    return 0;
}
