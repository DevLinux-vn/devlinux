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
int seats_sold = 0;
int failed_bookings = 0;
pthread_mutex_t seat_lock;

void* book_ticket(void* arg) {
    BookingRequest* req = (BookingRequest*)arg;

    printf("[Agent %d | TID %lu] Booking %d seats for %s...\n",
           req->agent_id, pthread_self(), req->seats_wanted, req->customer);

    // Sleep to force concurrency as requested
    sleep(1);

    pthread_mutex_lock(&seat_lock);

    /* 
     * WHY CHECK AND DEDUCT MUST BE IN THE SAME LOCK BLOCK:
     * If we checked seats available, unlocked, and then locked again to deduct, 
     * another thread could interleave, see the available seats, and deduct them 
     * before we relock. This would lead to overselling (race condition).
     * Therefore, the "check" and "deduct" operations must be grouped as a single 
     * atomic action inside the same critical section.
     */
    if (seats_available >= req->seats_wanted) {
        seats_available -= req->seats_wanted;
        seats_sold += req->seats_wanted;
        
        /*
         * NOTE (Performance Anti-pattern):
         * Placing I/O operations like printf() inside a critical section (mutex) is 
         * generally a bad practice in production, as I/O is slow and can block other 
         * threads, causing contention. It is done here strictly for educational 
         * purposes to ensure the log output remains sequential and non-interleaved.
         */
        printf("[Agent %d] CONFIRMED: %d seats for %s. Remaining: %d\n",
               req->agent_id, req->seats_wanted, req->customer, seats_available);
    } else {
        failed_bookings++;
        printf("[Agent %d] SOLD OUT:  needs %d seats, only %d left — booking failed.\n",
               req->agent_id, req->seats_wanted, seats_available);
    }

    pthread_mutex_unlock(&seat_lock);
    return NULL;
}

int main(void) {
    pthread_t threads[5];

    printf("==============================================\n");
    printf("   TICKET BOOKING SYSTEM (5 agents, 10 seats)\n");
    printf("==============================================\n");

    if (pthread_mutex_init(&seat_lock, NULL) != 0) {
        perror("Mutex init failed");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 5; i++) {
        if (pthread_create(&threads[i], NULL, book_ticket, (void*)&requests[i]) != 0) {
            perror("Thread creation failed");
            return EXIT_FAILURE;
        }
    }

    // Sleep 0.5s so threads print their first log, then print divider before they lock
    usleep(500000); 
    printf("\n--- [all agents reach critical section after sleep(1)] ---\n\n");

    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n================ SUMMARY ================\n");
    printf("  Total seats     : 10\n");
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Failed bookings : %d\n", failed_bookings);
    printf("=========================================\n");

    pthread_mutex_destroy(&seat_lock);
    return 0;
}
