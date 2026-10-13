#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

BookingRequest requests[5] = {
    {1, "Nguyen Van An", 2},
    {2, "Tran Thi Bich", 1},
    {3, "Le Van Cuong", 3},
    {4, "Pham Thi Dung", 1},
    {5, "Hoang Van Em", 2}
};

int seats_available = 10;
int total_seats = 10;
int failed_bookings = 0;

pthread_mutex_t seat_lock;

/*
 * Why must check and deduct be inside the SAME critical section?
 *
 * Wrong approach:
 *
 *   lock()
 *   enough = (seats_available >= wanted);
 *   unlock()
 *
 *   lock()
 *   seats_available -= wanted;
 *   unlock()
 *
 * Between the check and the deduction, another thread may change
 * seats_available. As a result, multiple threads can see enough seats
 * at the same time and all deduct later, causing overselling.
 *
 * Therefore, checking availability and deducting seats must be treated
 * as one atomic operation protected by a single lock/unlock pair.
 */

void *book_ticket(void *arg)
{
    BookingRequest *req = (BookingRequest *)arg;

    printf("[Agent %d | TID %lu] Booking %d seat%s for %s...\n",
           req->agent_id,
           (unsigned long)pthread_self(),
           req->seats_wanted,
           (req->seats_wanted > 1) ? "s" : "",
           req->customer);

    sleep(1);

    pthread_mutex_lock(&seat_lock);

    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;

        printf("[Agent %d] CONFIRMED: %d seat%s for %s. Remaining: %d\n",
               req->agent_id,
               req->seats_wanted,
               (req->seats_wanted > 1) ? "s" : "",
               req->customer,
               seats_available);
    } else {
        failed_bookings++;

        printf("[Agent %d] SOLD OUT: needs %d seat%s, only %d left - booking failed.\n",
               req->agent_id,
               req->seats_wanted,
               (req->seats_wanted > 1) ? "s" : "",
               seats_available);
    }

    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

int main(void)
{
    pthread_t threads[5];

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    pthread_mutex_init(&seat_lock, NULL);

    for (int i = 0; i < 5; i++) {
        if (pthread_create(&threads[i],
                           NULL,
                           book_ticket,
                           &requests[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    int seats_sold = total_seats - seats_available;

    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", total_seats);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    pthread_mutex_destroy(&seat_lock);

    return 0;
}