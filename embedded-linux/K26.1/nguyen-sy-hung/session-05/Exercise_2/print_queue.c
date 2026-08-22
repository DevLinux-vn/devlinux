#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define QUEUE_CAPACITY         5U
#define NUM_OF_PRODUCERS       3U
#define DOCS_PER_PRODUCER      3U
#define TOTAL_DOCUMENTS        (NUM_OF_PRODUCERS * DOCS_PER_PRODUCER)

#define FILE_NAME_LENGTH       60U
#define PRINT_DELAY_SEC        1U


typedef struct
{
    int doc_id;
    char filename[FILE_NAME_LENGTH];
    int pages;
} Document;


typedef struct
{
    int producer_id;
    Document documents[DOCS_PER_PRODUCER];
} ProducerData;


/* Shared print queue */
static Document queue[QUEUE_CAPACITY];

static int head = 0;
static int tail = 0;
static int count = 0;


/* Set by main after all producers finish */
static int all_sent = 0;


/* Summary data */
static int printed_documents = 0;
static int total_pages_printed = 0;


/* Synchronization objects */
static pthread_mutex_t q_lock;
static pthread_cond_t not_full;
static pthread_cond_t not_empty;


/*
 * pthread_cond_wait() must be used inside a while loop.
 *
 * Reason 1:
 * A thread can wake up even though no thread called
 * pthread_cond_signal(). This is called a spurious wakeup.
 *
 * Reason 2:
 * Multiple waiting threads may wake up and race to access
 * the queue. The condition must be checked again after
 * waking up to ensure it is still valid.
 */
static void enqueue_document(const Document *doc)
{
    queue[tail] = *doc;
    tail = (tail + 1) % QUEUE_CAPACITY;
    count++;
}


static Document dequeue_document(void)
{
    Document doc = queue[head];

    head = (head + 1) % QUEUE_CAPACITY;
    count--;

    return doc;
}


static void *producer(void *arg)
{
    ProducerData *data = (ProducerData *)arg;

    for (int i = 0; i < DOCS_PER_PRODUCER; i++)
    {
        pthread_mutex_lock(&q_lock);

        while (count == QUEUE_CAPACITY)
        {
            printf("[Producer %d] Queue full - waiting...\n",
                    data->producer_id);

            pthread_cond_wait(&not_full, &q_lock);
        }


        enqueue_document(&data->documents[i]);

        printf("[Producer %d] Submitting: %s (%d pages) - queue: %d/%d\n",
                data->producer_id,
                data->documents[i].filename,
                data->documents[i].pages,
                count,
                QUEUE_CAPACITY);


        pthread_cond_signal(&not_empty);

        pthread_mutex_unlock(&q_lock);
    }

    return NULL;
}


static void *printer(void *arg)
{
    (void)arg;

    while (1)
    {
        Document doc;


        pthread_mutex_lock(&q_lock);


        while ((count == 0) && (all_sent == 0))
        {
            pthread_cond_wait(&not_empty, &q_lock);
        }


        if ((count == 0) && (all_sent == 1))
        {
            pthread_mutex_unlock(&q_lock);
            break;
        }


        doc = dequeue_document();


        pthread_cond_signal(&not_full);


        pthread_mutex_unlock(&q_lock);


        printf("[Printer] Printing: %s (%d pages) - queue: %d/%d\n",
                doc.filename,
                doc.pages,
                count,
                QUEUE_CAPACITY);


        printed_documents++;
        total_pages_printed += doc.pages;


        sleep(PRINT_DELAY_SEC);
    }


    printf("[Printer] All documents printed. Exiting.\n");

    return NULL;
}


int main(void)
{
    pthread_t producers[NUM_OF_PRODUCERS];
    pthread_t printer_thread;


    ProducerData producer_data[NUM_OF_PRODUCERS] =
    {
        {
            1,
            {
                {1, "report_Q1.pdf", 12},
                {2, "slides.pdf", 20},
                {3, "summary.pdf", 4}
            }
        },

        {
            2,
            {
                {4, "contract.pdf", 5},
                {5, "memo.pdf", 2},
                {6, "budget.pdf", 7}
            }
        },

        {
            3,
            {
                {7, "invoice.pdf", 3},
                {8, "proposal.pdf", 8},
                {9, "plan.pdf", 5}
            }
        }
    };


    printf("============================================\n");
    printf(" OFFICE PRINT QUEUE\n");
    printf(" Producers: %d, Printer: 1\n",
            NUM_OF_PRODUCERS);

    printf(" Queue capacity: %d documents\n",
            QUEUE_CAPACITY);

    printf("============================================\n");


    pthread_mutex_init(&q_lock, NULL);

    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);


    for (int i = 0; i < NUM_OF_PRODUCERS; i++)
    {
        pthread_create(&producers[i],
                       NULL,
                       producer,
                       &producer_data[i]);
    }


    pthread_create(&printer_thread,
                   NULL,
                   printer,
                   NULL);


    for (int i = 0; i < NUM_OF_PRODUCERS; i++)
    {
        pthread_join(producers[i], NULL);
    }


    pthread_mutex_lock(&q_lock);

    all_sent = 1;

    pthread_cond_broadcast(&not_empty);

    pthread_mutex_unlock(&q_lock);


    pthread_join(printer_thread, NULL);


    pthread_mutex_destroy(&q_lock);

    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);


    printf("\n============== SUMMARY ==============\n");

    printf("Documents submitted : %d\n",
            TOTAL_DOCUMENTS);

    printf("Documents printed   : %d\n",
            printed_documents);

    printf("Total pages printed : %d\n",
            total_pages_printed);

    printf("=====================================\n");


    return 0;
}