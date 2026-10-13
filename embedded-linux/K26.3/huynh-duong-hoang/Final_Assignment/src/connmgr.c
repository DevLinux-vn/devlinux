#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "connmgr.h"
#include "protocol_adapter.h"
#include <errno.h>
#include <poll.h>
#include "sbuffer.h"

extern const protocol_adapter_t adapters[4];

void *connmgr_thread(void *arg){
    gateway_t *gateway = (gateway_t *)arg;

    printf(">>> FORCE DEBUG: base_port = %d <<<\n", gateway->base_port);

    logger_send("connmgr", "Connection manager thread started");

    int server_fds[4];
    struct pollfd fds[4];

    for (int i = 0; i < 4; i++){
        server_fds[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fds[i] < 0){
            perror("socket failed");
            continue;
        }

        int opt = 1;
        setsockopt(server_fds[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(gateway->base_port + adapters[i].port);

        int target_port = gateway->base_port + adapters[i].port;
        printf("DEBUG: Attempting to bind port %d\n", target_port);

        if (bind(server_fds[i], (struct sockaddr *)&address, sizeof(address)) < 0){
            perror("bind failed");
            close(server_fds[i]);
            server_fds[i] = -1;
            continue;
        }

        if (listen(server_fds[i], 5) < 0){
            perror("listen failed");
            close(server_fds[i]);
            server_fds[i] = -1;
            continue;
        }

        fds[i].fd = server_fds[i];
        fds[i].events = POLLIN;
        fds[i].revents = 0;
        
#ifdef DEBUG
        printf("Connection manager listening on port %d (adapter %d)\n", gateway->base_port + adapters[i].port, i);
#endif
    }

    while (!gateway->stop){
        int ret = poll(fds, 4, 500);
        if (ret < 0){
            if (errno == EINTR){
                continue;
            }
            break;
        } else if (ret == 0){
            continue;
        }

        for (int i = 0; i < 4; i++){
            if (fds[i].revents & POLLIN){
                struct sockaddr_in client_address;
                socklen_t client_address_len = sizeof(client_address);
                int client_fd = accept(fds[i].fd, (struct sockaddr *)&client_address, &client_address_len);
                
                if (client_fd >= 0){
                    char log_message[128];
                    snprintf(log_message, sizeof(log_message), "Connection accepted from %s:%d", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                    logger_send("connmgr", log_message);
#ifdef DEBUG
                    printf("DEBUG: Connection accepted on fd %d for adapter %d\n", client_fd, i);
#endif
                    
                    uint8_t buf[256];
                    ssize_t data_len = 0;

                    while(!gateway->stop){
                        ssize_t n = read(client_fd, buf + data_len, sizeof(buf) - (size_t)data_len - 1);
                        if (n <= 0){
                            break;
                        }

                        data_len += n;
                        buf[data_len] = '\0';
#ifdef DEBUG
                        printf("DEBUG: Received raw data (%zd bytes): %s\n", n, buf);
#endif

                        sensor_data_t sensor_data;
                        size_t consumed = 0;
                        parse_result_t result = adapters[i].parse(buf, (size_t)data_len, &sensor_data, &consumed);
                        
                        if (result == PARSE_OK){
                            uint8_t reply[8];
                            size_t reply_len = adapters[i].build_reply(result, reply, sizeof(reply));
                            if (reply_len > 0){
                                write(client_fd, reply, reply_len);
                            }

                            // Đưa dữ liệu vào sbuffer chia sẻ
                            sbuffer_insert(gateway->sbuffer, &sensor_data);

                            if ((ssize_t)consumed < data_len){
                                memmove(buf, buf + consumed, (size_t)data_len - consumed);
                                data_len -= (ssize_t)consumed;
                            } else {
                                data_len = 0;
                            }
                        } else if (result == PARSE_ERROR){
                            if ((ssize_t)consumed < data_len){
                                memmove(buf, buf + consumed, (size_t)data_len - consumed);
                                data_len -= (ssize_t)consumed;
                            } else {
                                data_len = 0;
                            }
                        } else {
                            // Nếu gói tin chưa hoàn chỉnh hoặc kết thúc, thoát vòng đọc nhỏ để đóng kết nối
                            break;
                        }
                    }
                    close(client_fd);
                    logger_send("connmgr", "Connection closed");
#ifdef DEBUG
                    printf("DEBUG: Connection closed for fd %d\n", client_fd);
#endif
                }
            }
        }
    }
    
    for (int i = 0; i < 4; i++){
        if (server_fds[i] >= 0){
            close(server_fds[i]);
        }
    }
#ifdef DEBUG
    printf("Connection manager thread stopped\n");
#endif
    logger_send("connmgr", "Connection manager thread stopped");
    return NULL;
}