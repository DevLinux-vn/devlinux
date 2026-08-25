#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define MAX_OF_CHAR_NAME 50
#define MAX_OF_SEATS 10

#define MAX_OF_AGENTS 5

//define struct
typedef struct {
    int agent_id;
    char customer[MAX_OF_CHAR_NAME];
    int seats_wanted;
}   BookingRequest;

int seats_available = MAX_OF_SEATS;
int seats_sold = 0;
int failed_booking = 0;

pthread_mutex_t seat_lock;

void *book_ticket(void *arg);

int main(void)  {

    int i;
    pthread_t agent_id[MAX_OF_AGENTS];

    BookingRequest requests[MAX_OF_AGENTS] = {
        {1, "Nguyen Van An",  2},
        {2, "Tran Thi Bich",  1},
        {3, "Le Van Cuong",   3},
        {4, "Pham Thi Dung",  1},
        {5, "Hoang Van Em",   2}
    };

    if (pthread_mutex_init(&seat_lock, NULL) != 0)  {
        perror("pthead_mutex_init");
        return EXIT_FAILURE;
    }

    printf("======================================================\n");
    printf("     TICKET BOOKING SYSTEM (%d agents, %d seats)      \n", \
                    (int)MAX_OF_AGENTS, (int)MAX_OF_SEATS);
    printf("======================================================\n");
    
    for (i = 0; i < MAX_OF_AGENTS; i++) {
        if(pthread_create(&agent_id[i], NULL, book_ticket, &requests[i]) != 0) {
            perror("pthread_create");
            pthread_mutex_destroy(&seat_lock);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < MAX_OF_AGENTS; i++) {
        if (pthread_join(agent_id[i], NULL) != 0)  {
            perror("pthread_join");
            pthread_mutex_destroy(&seat_lock);
            return EXIT_FAILURE;
        }
    }

    printf("=============== SUMMARY ================\n");
    printf("  Total seats     : %d\n", (int)MAX_OF_SEATS);
    printf("  Seats sold      : %d\n", seats_sold);
    printf("  Seats remaining : %d\n", seats_available);
    printf("  Fail booking    : %d\n", failed_booking);
    printf("=========================================\n");

    return EXIT_SUCCESS;
}

void *book_ticket(void *arg) {
    
    BookingRequest *book = (BookingRequest *)arg;
    unsigned long TID = (unsigned long)pthread_self();

    sleep(1);

    printf("[Agent %d | TID %lu] Booking %d seat%s for %s...\n",
            book->agent_id,
            TID,
            book->seats_wanted,
            book->seats_wanted == 1 ? "" : "s",
            book->customer
    );
    
    pthread_mutex_lock(&seat_lock);

    if (seats_available >= book->seats_wanted)  {
        
        seats_available -= book->seats_wanted;
        seats_sold += book->seats_wanted;

        printf("[Agent %d] CONFIRMED: %d seat%s for %s. Remaining: %d\n",
                book->agent_id,
                book->seats_wanted,
                book->seats_wanted == 1 ? "" : "s",
                book->customer,
                seats_available
        );

    }   else  {

        failed_booking++;

        printf("[Agent %d] SOLD OUT: needs %d seat%s, only %d left - booking fail\n",
                book->agent_id,
                book->seats_wanted,
                book->seats_wanted == 1 ? "" : "s",
                seats_available
        );
    }

    pthread_mutex_unlock(&seat_lock);

    return NULL;
}

