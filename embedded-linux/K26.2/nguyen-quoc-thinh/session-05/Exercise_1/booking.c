#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    int  agent_id;
    char customer[50];
    int  seats_wanted;
} BookingRequest;

int seats_available = 10;
int seats_sold = 0;
int failed_bookings = 0;
pthread_mutex_t seat_lock;

void *book_ticket(void *arg) {
    BookingRequest *req = (BookingRequest *)arg;

    printf("[Agent %d | TID %lu] Booking %d seat%s for %s...\n",
           req->agent_id, (unsigned long)pthread_self(),
           req->seats_wanted, req->seats_wanted > 1 ? "s" : " ",
           req->customer);

    sleep(1);

    pthread_mutex_lock(&seat_lock);
    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;
        seats_sold += req->seats_wanted;
        printf("[Agent %d] CONFIRMED: %d seat%s for %s. Remaining: %d\n",
               req->agent_id, req->seats_wanted,
               req->seats_wanted > 1 ? "s" : " ",
               req->customer, seats_available);
    } else {
        failed_bookings++;
        printf("[Agent %d] SOLD OUT: needs %d seats, only %d left - booking failed.\n",
               req->agent_id, req->seats_wanted, seats_available);
    }
    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

int main(void) {
    BookingRequest requests[5] = {
        {1, "Nguyen Van An",  2},
        {2, "Tran Thi Bich",  1},
        {3, "Le Van Cuong",   3},
        {4, "Pham Thi Dung",  1},
        {5, "Hoang Van Em",   2}
    };

    pthread_t threads[5];
 
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    pthread_mutex_init(&seat_lock, NULL);

    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, book_ticket, &requests[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n SUMMARY \n");
    printf("  Total seats     : 10\n");
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);

    pthread_mutex_destroy(&seat_lock);

    return 0;
}