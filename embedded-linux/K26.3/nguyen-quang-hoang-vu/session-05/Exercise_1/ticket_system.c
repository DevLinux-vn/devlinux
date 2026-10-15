#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define INITIAL_TICKETS 20
#define TOTAL_CUSTOMERS 30

static int available_tickets =
    INITIAL_TICKETS;

pthread_mutex_t ticket_mutex;

void *book_ticket(void *arg)
{
    int customer_id =
        *(int *)arg;

    if (pthread_mutex_lock(
            &ticket_mutex)
        != 0)
    {
        perror(
            "pthread_mutex_lock");

        return NULL;
    }

    printf(
        "[CUSTOMER-%d] Trying to book ticket\n",
        customer_id);

    if (available_tickets > 0)
    {
        available_tickets--;

        printf(
            "[CUSTOMER-%d] Booked successfully. Remaining=%d\n",
            customer_id,
            available_tickets);
    }
    else
    {
        printf(
            "[CUSTOMER-%d] Sold out\n",
            customer_id);
    }

    if (pthread_mutex_unlock(
            &ticket_mutex)
        != 0)
    {
        perror(
            "pthread_mutex_unlock");
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[TOTAL_CUSTOMERS];

    int ids[TOTAL_CUSTOMERS];

    if (pthread_mutex_init(
            &ticket_mutex,
            NULL)
        != 0)
    {
        perror(
            "pthread_mutex_init");

        return 1;
    }

    for (int i = 0;
         i < TOTAL_CUSTOMERS;
         i++)
    {
        ids[i] = i + 1;

        printf(
            "[MAIN] Creating customer thread %d\n",
            ids[i]);

        if (pthread_create(
                &threads[i],
                NULL,
                book_ticket,
                &ids[i])
            != 0)
        {
            perror(
                "pthread_create");
        }
    }

    for (int i = 0;
         i < TOTAL_CUSTOMERS;
         i++)
    {
        if (pthread_join(
                threads[i],
                NULL)
            != 0)
        {
            perror(
                "pthread_join");
        }
    }

    printf("\n");
    printf("===== SUMMARY =====\n");

    printf(
        "Initial tickets : %d\n",
        INITIAL_TICKETS);

    printf(
        "Final tickets   : %d\n",
        available_tickets);

    printf(
        "Sold tickets    : %d\n",
        INITIAL_TICKETS -
        available_tickets);

    if (pthread_mutex_destroy(
            &ticket_mutex)
        != 0)
    {
        perror(
            "pthread_mutex_destroy");
    }

    return 0;
}
