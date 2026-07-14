#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define TOTAL_SEATS        10U
#define NUM_OF_AGENTS      5U
#define CUSTOMER_NAME_LEN  50U
#define PROCESS_DELAY_SEC  1U

typedef struct {
    int  agent_id;
    char customer[CUSTOMER_NAME_LEN];
    int  seats_wanted;
} BookingRequest;

static int seats_available = TOTAL_SEATS;
static int failed_bookings = 0;

static pthread_mutex_t seat_lock;

/*
 * The checking and deducting operations must be protected
 * by the same critical section.
 *
 * If they are separated into different lock/unlock blocks,
 * multiple threads may read the same available seat count
 * before any deduction happens, causing overselling.
 */
void *book_ticket(void *arg)
{
    BookingRequest *request = (BookingRequest *)arg;

    printf("[Agent %d | TID %lu] Request %d seats for %s\n",
            request->agent_id,
            pthread_self(),
            request->seats_wanted,
            request->customer);

    sleep(PROCESS_DELAY_SEC);


    pthread_mutex_lock(&seat_lock);

    if (seats_available >= request->seats_wanted)
    {
        seats_available -= request->seats_wanted;

        printf("[Agent %d] CONFIRMED: %d seats for %s. Remaining: %d\n",
                request->agent_id,
                request->seats_wanted,
                request->customer,
                seats_available);
    }
    else
    {
        failed_bookings++;

        printf("[Agent %d] FAILED: Need %d seats, only %d left.\n",
                request->agent_id,
                request->seats_wanted,
                seats_available);
    }

    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

int main(void)
{
    pthread_t agents[NUM_OF_AGENTS];


    BookingRequest requests[NUM_OF_AGENTS] =
    {
        {1, "Nguyen Van An", 2},
        {2, "Tran Thi Bich", 1},
        {3, "Le Van Cuong", 3},
        {4, "Pham Thi Dung", 1},
        {5, "Hoang Van Em",  5}
    };


    printf("========================================\n");
    printf("   TICKET BOOKING SYSTEM\n");
    printf("========================================\n");


    pthread_mutex_init(&seat_lock, NULL);


    for (int i = 0; i < NUM_OF_AGENTS; i++)
    {
        pthread_create(&agents[i],
                       NULL,
                       book_ticket,
                       &requests[i]);
    }


    for (int i = 0; i < NUM_OF_AGENTS; i++)
    {
        pthread_join(agents[i], NULL);
    }


    pthread_mutex_destroy(&seat_lock);


    printf("\n============= SUMMARY =============\n");
    printf("Total seats     : %d\n", TOTAL_SEATS);
    printf("Seats sold      : %d\n",
            TOTAL_SEATS - seats_available);
    printf("Seats remaining : %d\n",
            seats_available);
    printf("Failed bookings : %d\n",
            failed_bookings);
    printf("===================================\n");


    return 0;
}