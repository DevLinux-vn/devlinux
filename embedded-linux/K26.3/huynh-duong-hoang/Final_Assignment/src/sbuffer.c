#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sbuffer.h"

int sbuffer_init(sbuffer_t **buffer){
    *buffer = malloc(sizeof(sbuffer_t));
    if (buffer == NULL){
        return -1;
    }

    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    (*buffer)->size = 0;
    (*buffer)->shutdown = false;

    pthread_mutex_init(&((*buffer)->lock), NULL);
    pthread_cond_init(&((*buffer)->not_empty), NULL);
    
    free(*buffer);
    *buffer = NULL;

    return 0;
}
int sbuffer_insert(sbuffer_t *buffer, const sensor_data_t *data){
    if (buffer == NULL || data == NULL){
        return -1;
    }

    sbuffer_node_t *new_node = malloc(sizeof(sbuffer_node_t));
    if (new_node == NULL){
        return -1;
    }
    new_node->data = *data;
    new_node->read_by_data = false;
    new_node->read_by_storage = false;
    new_node->next = NULL;

    pthread_mutex_lock(&buffer->lock);

    if (buffer->head == NULL){
        buffer->head = new_node;
        buffer->tail = new_node;
    }else{
        buffer->tail->next = new_node;
        buffer->tail = new_node;
    }
    buffer->size++;
    pthread_cond_broadcast(&buffer->not_empty);
    pthread_mutex_unlock(&buffer->lock);

    return 0;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data, int reader_type){
    if (buffer == NULL || data == NULL){
        return -1;
    }

    pthread_mutex_lock(&(buffer->lock));

    while (buffer->head == NULL && !buffer->shutdown){
        pthread_cond_wait(&(buffer->not_empty), &(buffer->lock));
    }

    if (buffer->head == NULL && buffer->shutdown){
        pthread_mutex_unlock(&(buffer->lock));
        return 1;
    }

    sbuffer_node_t *current = buffer->head;
    *data = current->data;

    if (reader_type == 1){
        current->read_by_data = true;
    }else if (reader_type == 2){
        current->read_by_storage = true;
    }

    if (current->read_by_data && current->read_by_storage){
        buffer->head = current->next;
        if (buffer->head == NULL){
            buffer->tail = NULL;
        }
        buffer->size--;
        free(current);
    }
    pthread_mutex_unlock(&(buffer->lock));
    return 0;
}

int sbuffer_set_shutdown(sbuffer_t *buffer){
    if (buffer == NULL){
        return -1;
    }

    pthread_mutex_lock(&(buffer->lock));
    buffer->shutdown = true;
    pthread_cond_broadcast(&(buffer->not_empty));
    pthread_mutex_unlock(&(buffer->lock));
    return 0;
}