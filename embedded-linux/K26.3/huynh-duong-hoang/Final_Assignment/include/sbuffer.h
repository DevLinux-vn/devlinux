#ifndef SBUFFER_H
#define SBUFFER_H

#include <stdbool.h>
#include "sensor_types.h"

typedef struct sbuffer_node{
    sensor_data_t data;
    bool read_by_data;
    bool read_by_storage;
    struct sbuffer_node *next;
} sbuffer_node_t;

typedef struct{
    sbuffer_node_t *head;
    sbuffer_node_t *tail;
    size_t size;
    bool shutdown;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
} sbuffer_t;

int sbuffer_init(sbuffer_t **buffer);
int sbuffer_free(sbuffer_t **buffer);
int sbuffer_insert(sbuffer_t *buffer, const sensor_data_t *data);
int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data, int reader_type);
int sbuffer_set_shutdown(sbuffer_t *buffer);

#endif /* SBUFFER_H */