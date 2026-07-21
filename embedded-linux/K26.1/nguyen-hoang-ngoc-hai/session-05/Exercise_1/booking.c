/*
Why check and deduct must be in the same lock/unlock block:

The condition check:

    if (seats_available >= seats_wanted)

and the update:

    seats_available -= seats_wanted;

must be protected by the same mutex critical section.

If we lock only for checking, then unlock, and later lock again for deducting,
another thread may change seats_available between those two steps.

Example:
- seats_available = 2
- Agent A checks: enough seats
- Agent B checks: enough seats
- Agent A deducts 2 seats
- Agent B also deducts 2 seats

Result: the system sells more seats than available.

Therefore, check-and-deduct must be atomic.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define TOTAL_SEATS 10
#define REQUEST_COUNT 5

typedef struct {
    int agent_id;
    char customer[50];
    int seats_wanted;
} BookingRequest;

int seats_available = TOTAL_SEATS;
int failed_bookings = 0;

pthread_mutex_t seat_lock;

void *book_ticket(void *arg) {
    BookingRequest *request = (BookingRequest *)arg;

    printf("[Agent %d | TID %lu] Booking %d seat%s for %s...\n",
           request->agent_id,
           (unsigned long)pthread_self(),
           request->seats_wanted,
           request->seats_wanted > 1 ? "s" : "",
           request->customer);

    sleep(1);

    pthread_mutex_lock(&seat_lock);

    if (seats_available >= request->seats_wanted) {
        seats_available -= request->seats_wanted;

        printf("[Agent %d] CONFIRMED: %d seat%s for %s. Remaining: %d\n",
               request->agent_id,
               request->seats_wanted,
               request->seats_wanted > 1 ? "s" : "",
               request->customer,
               seats_available);
    } else {
        failed_bookings++;

        printf("[Agent %d] SOLD OUT: needs %d seat%s, only %d left — booking failed.\n",
               request->agent_id,
               request->seats_wanted,
               request->seats_wanted > 1 ? "s" : "",
               seats_available);
    }

    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

int main(void) {
    BookingRequest requests[REQUEST_COUNT] = {
        {1, "Nguyen Van An",  2},
        {2, "Tran Thi Bich",  1},
        {3, "Le Van Cuong",   3},
        {4, "Pham Thi Dung",  1},
        {5, "Hoang Van Em",   2}
    };

    pthread_t threads[REQUEST_COUNT];

    pthread_mutex_init(&seat_lock, NULL);

    printf("\n==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    for (int i = 0; i < REQUEST_COUNT; i++) {
        if (pthread_create(&threads[i], NULL, book_ticket, &requests[i]) != 0) {
            perror("pthread_create");
            failed_bookings++;
        }
    }

    for (int i = 0; i < REQUEST_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    int seats_sold = TOTAL_SEATS - seats_available;

    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", TOTAL_SEATS);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    pthread_mutex_destroy(&seat_lock);

    return 0;
}