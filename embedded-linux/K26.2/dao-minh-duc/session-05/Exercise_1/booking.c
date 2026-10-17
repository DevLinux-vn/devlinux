#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 * The check and deduct operations must be inside the same
 * mutex lock/unlock block because they form one atomic
 * critical section.
 *
 * If the check and deduct were protected by two separate
 * lock/unlock operations, another thread could change
 * seats_available between the check and the deduction.
 * This could cause multiple threads to see the same number
 * of available seats and result in incorrect seat counts
 * or overbooking.
 *
 * Therefore, the correct sequence is:
 *
 *     pthread_mutex_lock(&seat_lock);
 *         check available seats;
 *         deduct requested seats;
 *     pthread_mutex_unlock(&seat_lock);
 */

typedef struct {
    int agent_id;
    char customer[50];
    int seats_wanted;
} BookingRequest;

BookingRequest requests[5] = {
    {1, "Nguyen Van An",  2},
    {2, "Tran Thi Bich",   1},
    {3, "Le Van Cuong",    3},
    {4, "Pham Thi Dung",   1},
    {5, "Hoang Van Em",    2}
};

int seats_available = 10;
pthread_mutex_t seat_lock;

void *book_ticket(void *arg)
{
    BookingRequest *request = (BookingRequest *)arg;

    sleep(1);

    printf("[Agent %d | TID %lu] Booking %d seat%s for %s...\n",
           request->agent_id,
           (unsigned long)pthread_self(),
           request->seats_wanted,
           request->seats_wanted == 1 ? "" : "s",
           request->customer);

    pthread_mutex_lock(&seat_lock);

    if (seats_available >= request->seats_wanted) {
        seats_available -= request->seats_wanted;

        printf("[Agent %d] CONFIRMED: %d seat%s for %s. Remaining: %d\n",
               request->agent_id,
               request->seats_wanted,
               request->seats_wanted == 1 ? "" : "s",
               request->customer,
               seats_available);
    } else {
        printf("[Agent %d] SOLD OUT: needs %d seats, only %d left - booking failed.\n",
               request->agent_id,
               request->seats_wanted,
               seats_available);
    }

    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

int main(void)
{
    pthread_t threads[5];

    int total_seats = 10;
    int seats_sold;
    int failed_bookings = 0;

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    if (pthread_mutex_init(&seat_lock, NULL) != 0) {
        perror("pthread_mutex_init");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 5; i++) {
        if (pthread_create(&threads[i],
                           NULL,
                           book_ticket,
                           &requests[i]) != 0) {
            perror("pthread_create");
            pthread_mutex_destroy(&seat_lock);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            pthread_mutex_destroy(&seat_lock);
            return EXIT_FAILURE;
        }
    }

    seats_sold = total_seats - seats_available;

    for (int i = 0; i < 5; i++) {
        if (requests[i].seats_wanted > total_seats) {
            failed_bookings++;
        }
    }

    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : %d\n", total_seats);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    pthread_mutex_destroy(&seat_lock);

    return EXIT_SUCCESS;
}
