#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>

#define MAX_CHAR_OF_FILE_NAME 60

#define MAX_OF_PRODUCER 3

#define MAX_OF_SUBMIT_PER_PRODUCER 3

#define MAX_OF_PRINTER 1

#define MAX_OF_DOCUMENTS_PER_QUEUE 5

#define MAX_OF_DOCUMENTS 9

//define struct
typedef struct {
    int  doc_id;
    char filename[MAX_CHAR_OF_FILE_NAME];
    int  pages;
} Document;

typedef struct  {
    int head;
    int tail;
    int count;
    Document docs[MAX_OF_DOCUMENTS_PER_QUEUE];
} DocumentQueue;

int all_sent = 0;
int submitted = 0;
int printed = 0;
int total_page = 0;

pthread_mutex_t q_lock;
pthread_cond_t not_full;
pthread_cond_t not_empty;

DocumentQueue docs_queue = {
    .count = 0,
    .head = 0,
    .tail = 0,
    .docs = {}
};

void *producer(void *arg);
void *printer(void *arg);

int main()  {
    
    int err;    
    int i;
    
    pthread_t producer_id[MAX_OF_PRODUCER];
    pthread_t printer_id[MAX_OF_PRINTER];

    err = pthread_mutex_init(&q_lock, NULL);
    if (err != 0)   {
        fprintf(stderr, "pthread_mutex_init q_lock failed\n");
        return EXIT_FAILURE;
    }

    err = pthread_cond_init(&not_full, NULL);
    if (err != 0)   {
        fprintf(stderr, "pthread_cond_init not_full failed\n");
        pthread_mutex_destroy(&q_lock);
        return EXIT_FAILURE;
    }

    err = pthread_cond_init(&not_empty, NULL);
    if (err != 0)   {
        fprintf(stderr, "pthread_cond_init not_emty failed\n");
        pthread_mutex_destroy(&q_lock);
        pthread_cond_destroy(&not_full);
        return EXIT_FAILURE;
    }

    Document producer_doc[MAX_OF_PRODUCER][MAX_OF_SUBMIT_PER_PRODUCER] = {
        {
            {1, "report_01.pdf", 12},
            {2, "slides.pdf", 20},
            {3, "summary.pdf", 4}
        },
        {
            {4, "contract.pdf", 5},
            {5, "memo.pdf", 2},
            {6, "budget.pdf", 7}
        },
        {
            {7, "invoice.pdf", 3},
            {8, "proposal.pdf", 8},
            {9, "design.pdf", 1}
        }
    };

    printf("==================================================\n");
    printf(" OFFICE PRINT QUEUE (%d producer%s, %d printer%s) \n",
            (int)MAX_OF_PRODUCER, 
            (int)MAX_OF_PRODUCER == 1 ? "" : "s",
            (int)MAX_OF_PRINTER,
            (int)MAX_OF_PRINTER == 1 ? "" : "s"
    );
    printf(" Queue capacity: %d document%s                    \n",
            (int)MAX_OF_DOCUMENTS_PER_QUEUE,
            (int)MAX_OF_DOCUMENTS_PER_QUEUE == 1 ? "" : "s"
    );
    printf("==================================================\n");

    for (i = 0; i < MAX_OF_PRODUCER; i++)   {
        err = pthread_create(&producer_id[i], NULL, producer, &producer_doc[i]);
        if (err != 0)   {
            fprintf(stderr, "pthread_create producer failed\n");
            pthread_mutex_destroy(&q_lock);
            pthread_cond_destroy(&not_full);
            pthread_cond_destroy(&not_empty);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < MAX_OF_PRINTER; i++)    {
        err = pthread_create(&printer_id[i], NULL, printer, NULL);
        if (err != 0)   {
            fprintf(stderr, "pthread_create printer failed\n");
            pthread_mutex_destroy(&q_lock);
            pthread_cond_destroy(&not_full);
            pthread_cond_destroy(&not_empty);
            return EXIT_FAILURE;
        }
    }

    /*
     *Waiting all producer return
     */
    for (i = 0; i < MAX_OF_PRODUCER; i++)   {
        err = pthread_join(producer_id[i], NULL);
        if (err != 0)   {
            fprintf(stderr, "pthread_join producer failed\n");
            pthread_mutex_destroy(&q_lock);
            pthread_cond_destroy(&not_full);
            pthread_cond_destroy(&not_empty);
            return EXIT_FAILURE;
        }
    }

    /*
     * All producers have finished submitting documents.
     *
     * The printer may be sleeping because the queue is empty.
     * Set all_sent and wake the printer so it can check
     * whether there are still documents to print.
     */
    pthread_mutex_lock(&q_lock);
    all_sent = 1;
    pthread_cond_broadcast(&not_empty);
    pthread_mutex_unlock(&q_lock);

    for (i = 0; i < MAX_OF_PRINTER; i++)   {
        err = pthread_join(printer_id[i], NULL);
        if (err != 0)   {
            fprintf(stderr, "pthread_join printer failed\n");
            pthread_mutex_destroy(&q_lock);
            pthread_cond_destroy(&not_full);
            pthread_cond_destroy(&not_empty);
            return EXIT_FAILURE;
        }
    }

    printf("=============== SUMMARY ===============\n");
    printf("  Document%s submitted : %d            \n", submitted == 1 ? "" : "s", submitted);
    printf("  Document%s printed   : %d            \n", printed == 1 ? "" : "s", printed);
    printf("  Total page%s printed : %d            \n", total_page == 1 ? "" : "s", total_page);
    printf("=======================================\n");

    pthread_mutex_destroy(&q_lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
    return EXIT_SUCCESS;
}

void *producer(void *arg)   {
    
    //Receive arg value as know as Document 
    Document *docs = (Document *)arg;

    //Get pthread_id
    unsigned long ID = (unsigned long)pthread_self();

    //
    for (int i = 0; i < MAX_OF_SUBMIT_PER_PRODUCER; i++)    {
        
        Document doc = docs[i];

        pthread_mutex_lock(&q_lock);
        
        //waiting
        if (docs_queue.count == MAX_OF_DOCUMENTS_PER_QUEUE) {
            printf("[Producer %lu] Queue full - waiting...\n", ID);
        }

        /*
         * If the queue is full, this producer must wait.
         *
         * pthread_cond_wait() must be inside a while loop.
         *
         * A condition variable can have a "spurious wakeup":
         * a thread can wake up even though the condition it
         * was waiting for is still false.
         *
         * Therefore, after waking up, the thread must check
         * the condition again.
         */
        while(docs_queue.count == MAX_OF_DOCUMENTS_PER_QUEUE) {
            pthread_cond_wait(&not_full, &q_lock);
        }
        
        //push doc to queue
        docs_queue.docs[docs_queue.tail] = doc;
        
        //
        docs_queue.tail = (docs_queue.tail + 1) % MAX_OF_DOCUMENTS_PER_QUEUE;
        //
        docs_queue.count++;

        printf("[Producer %lu] Submitting: %s (%d page%s) - queue: %d/%d\n",
                ID,
                doc.filename,
                doc.pages,
                doc.pages == 1 ? "":"s",
                docs_queue.count,
                MAX_OF_DOCUMENTS_PER_QUEUE
        );
        
        //
        submitted++;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&q_lock);
    }

    return NULL;
}

void *printer(void *arg)    {

    //unused arg
    (void)arg;

    unsigned long ID = (unsigned long)pthread_self();

    while(1)    {

        pthread_mutex_lock(&q_lock);
        
        /*
         * Wait while:
         *
         * 1. Queue is empty
         * 2. Producers have not finished yet
         *
         * The while loop is important because of spurious
         * wakeups and because another thread may consume the
         * available condition before this thread runs.
         */
        while((docs_queue.count == 0) && (all_sent != 1)) {
            pthread_cond_wait(&not_empty, &q_lock);
        }

        if ((docs_queue.count == 0) && (all_sent == 1))   {
            printf("[Printer %lu] ALL document%s printed. Exiting.\n",
                    ID,
                    printed == 1 ? "" : "s"
            );
            pthread_mutex_unlock(&q_lock);
            break;
        }

        docs_queue.count--;

        printf("[Printer %lu] Printing: %s (%d page%s) - queue: %d/%d\n",
            ID,
            docs_queue.docs[docs_queue.head].filename,
            docs_queue.docs[docs_queue.head].pages,
            docs_queue.docs[docs_queue.head].pages == 1 ? "" : "s",
            docs_queue.count,
            (int)MAX_OF_DOCUMENTS_PER_QUEUE
        );        
        
        printed++;
        total_page += docs_queue.docs[docs_queue.head].pages;

        docs_queue.head = (docs_queue.head + 1) % MAX_OF_DOCUMENTS_PER_QUEUE;

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&q_lock);
        sleep(2);
    }

    return NULL;
}

