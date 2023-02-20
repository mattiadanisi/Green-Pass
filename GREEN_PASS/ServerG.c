//
// Created by Mattia Danisi on 17/02/23.
//

#include "ServerG.h"

volatile sig_atomic_t running = 1;

void *handle_connection(void *);
void sigint_handler(int);


int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    pthread_t thread_pool[THREAD_POOL_SIZE];

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);


    // create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }


    // bind server socket to a port
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // listen on all available network interfaces
    server_address.sin_port = htons(SERVER_G_PORT); // listen on port 1025
    if (bind(server_socket, (SA *) &server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(2);
    }

    // listen for connections
    if (listen(server_socket, SOMAXCONN) == -1) {
        perror("listen");
        exit(3);
    }

    printf("server G listening on port %d\n", SERVER_G_PORT);

    // initialize thread pool
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (pthread_create(&thread_pool[i], NULL, handle_connection, NULL) != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    // wait for connections and assign to threads in pool
    while (running) {
        client_address_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *) &client_address, &client_address_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        // find first available thread in pool
        int thread_index = -1;
        for (int i = 0; i < THREAD_POOL_SIZE; i++) {
            if (pthread_kill(thread_pool[i], 0) == ESRCH) {
                // thread is available
                thread_index = i;
                break;
            }
        }

        if (thread_index == -1) {
            // no threads available, close connection
            close(client_socket);
        } else {
            // pass client socket to thread
            int *client_socket_ptr = malloc(sizeof(int));
            *client_socket_ptr = client_socket;
            if (pthread_create(&thread_pool[thread_index], NULL, handle_connection, client_socket_ptr) != 0) {
                perror("pthread_create");
                break;
            } else {
                // detach thread so it can run in the background
                if (pthread_detach(thread_pool[thread_index]) != 0) {
                    perror("pthread_detach");
                    break;
                }
            }
        }
    }

    close(client_socket);

    return 0;
}


void *handle_connection(void *arg) {
    // Get client socket
    int client_socket;
    if (arg == NULL) {
        // handle error
        return NULL;
    } else {
        client_socket = *((int *) arg);
        free(arg);
    }

    // Receive data from client
    struct green_pass_check gpc;
    if(recv(client_socket, &gpc, sizeof(gpc), 0) == -1 ){
        perror("recv");
        close(client_socket);
        return NULL;
    }

    printf("code received: %s\n", gpc.card);

    // Connect to server_v
    int serverV_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverV_socket == -1) {
        perror("socket");
        close(client_socket);
        return NULL;
    }

    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    server_address.sin_port = htons(SERVER_V_PORT);

    if (connect(serverV_socket, (SA *)&server_address, sizeof(server_address)) == -1) {
        perror("connect");
        close(client_socket);
        close(serverV_socket);
        return NULL;
    }

    if(gpc.service == SERVICE_READ_GP) {
        printf("service requested: read\n");
        //generate green pass
        struct green_pass gp;
        stpcpy(gp.card, gpc.card);
        gp.service = SERVICE_READ_GP;

        // Send data to server V
        if (send(serverV_socket, &gp, sizeof(gp), 0) == -1) {
            perror("send");
            close(client_socket);
            close(serverV_socket);
            return NULL;
        }


        char response[MAX_RESPONSE_SIZE];
        if (recv(serverV_socket, response, MAX_RESPONSE_SIZE, 0) == -1) {
            perror("recv");
            close(client_socket);
            close(serverV_socket);
            return NULL;
        }

        printf("green pass is %s\n", response);

        if(send(client_socket, response, MAX_RESPONSE_SIZE, 0) == -1) {
            perror("send");
            close(client_socket);
            close(serverV_socket);
            return NULL;
        }
    }

    else if (gpc.service == SERVICE_VALIDITY_GP) {
        printf("service requested: validity\n");
        //generate green pass
        struct green_pass gp;
        stpcpy(gp.card, gpc.card);
        gp.service = SERVICE_VALIDITY_GP;

        // Send data to server V
        if (send(serverV_socket, &gp, sizeof(gp), 0) == -1) {
            perror("send");
            close(client_socket);
            close(serverV_socket);
            return NULL;
        }
    }

    // Close sockets
    close(serverV_socket);
    close(client_socket);
    return NULL;
}

void sigint_handler(int sig) {
    running = 0;
}
