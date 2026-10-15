#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define PRODUCE_COUNT 20

static int buffer = 0;

pthread_mutex_t mutex;
pthread_cond_t not_empty;
pthread_cond_t not_full;

void *producer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= PRODUCE_COUNT; i++)
    {
        if (pthread_mutex_lock(&mutex) != 0)
        {
            perror("pthread_mutex_lock");
            return NULL;
        }

        while (buffer >= BUFFER_SIZE)
        {
            printf("[PRODUCER] Buffer full, waiting...\n");

            pthread_cond_wait(
                &not_full,
                &mutex);
        }

        buffer++;

        printf(
            "[PRODUCER] Produced item %d, buffer=%d\n",
            i,
            buffer);

        pthread_cond_signal(
            &not_empty);

        if (pthread_mutex_unlock(
                &mutex)
            != 0)
        {
            perror(
                "pthread_mutex_unlock");
        }

        usleep(100000);
    }

    return NULL;
}

void *consumer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= PRODUCE_COUNT; i++)
    {
        if (pthread_mutex_lock(
                &mutex)
            != 0)
        {
            perror(
                "pthread_mutex_lock");

            return NULL;
        }

        while (buffer <= 0)
        {
            printf(
                "[CONSUMER] Buffer empty, waiting...\n");

            pthread_cond_wait(
                &not_empty,
                &mutex);
        }

        buffer--;

        printf(
            "[CONSUMER] Consumed item %d, buffer=%d\n",
            i,
            buffer);

        pthread_cond_signal(
            &not_full);

        if (pthread_mutex_unlock(
                &mutex)
            != 0)
        {
            perror(
                "pthread_mutex_unlock");
        }

        usleep(150000);
    }

    return NULL;
}

int main(void)
{
    pthread_t producer_thread;
    pthread_t consumer_thread;

    if (pthread_mutex_init(
            &mutex,
            NULL)
        != 0)
    {
        perror(
            "pthread_mutex_init");

        return 1;
    }

    if (pthread_cond_init(
            &not_empty,
            NULL)
        != 0)
    {
        perror(
            "pthread_cond_init");

        return 1;
    }

    if (pthread_cond_init(
            &not_full,
            NULL)
        != 0)
    {
        perror(
            "pthread_cond_init");

        return 1;
    }

    printf(
        "[MAIN] Starting producer thread\n");

    pthread_create(
        &producer_thread,
        NULL,
        producer,
        NULL);

    printf(
        "[MAIN] Starting consumer thread\n");

    pthread_create(
        &consumer_thread,
        NULL,
        consumer,
        NULL);

    pthread_join(
        producer_thread,
        NULL);

    pthread_join(
        consumer_thread,
        NULL);

    printf("\n");
    printf("===== SUMMARY =====\n");
    printf("Final buffer = %d\n",
           buffer);

    pthread_cond_destroy(
        &not_empty);

    pthread_cond_destroy(
        &not_full);

    pthread_mutex_destroy(
        &mutex);

    return 0;
}
