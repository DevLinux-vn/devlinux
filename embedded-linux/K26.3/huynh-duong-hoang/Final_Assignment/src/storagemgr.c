#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "storagemgr.h"
#include "gateway.h"

void *storagemgr_thread(void *arg){
    gateway_t *gateway = (gateway_t *)arg;
#ifdef DEBUG
    printf("Storage manager thread started\n");
#endif
    logger_send("storagemgr", "Storage manager thread started");

    FILE *db_file = fopen("gateway.db", "w");
    if (db_file == NULL){
        printf("Error opening database file\n");
        exit(1);
    }

    while (!gateway->stop){
        sensor_data_t sensor_data;
        int res = sbuffer_remove(gateway -> sbuffer, &sensor_data, 2);
        if (res == 1){
            break;
        }else if (res == 0){
            if(db_file){
                fprintf(db_file, "%d,%d,%.2f\n", sensor_data.id, sensor_data.type, sensor_data.value);
                fflush(db_file);
        }
    }
}

if (db_file){
    fclose(db_file);
}
#ifdef DEBUG
    printf("Storage manager thread stopped\n");
#endif
    logger_send("storagemgr", "Storage manager thread stopped");
    return NULL;
}