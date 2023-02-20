//
// Created by Mattia Danisi on 19/02/23.
//

#include "Shared.h"

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("Correct usage: ./ClientS <TS_CODE>\n");
        exit(1);
    }

    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(2);
    }

    // Connect to server G
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_G_PORT);
    server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_socket);
        exit(3);
    }

    struct green_pass_check gpc;
    strcpy(gpc.card, argv[1]);
    gpc.service = SERVICE_READ_GP;

    //send code to server G
    if(send(client_socket, &gpc, sizeof(gpc),0) == -1) {
        perror("send");
        close(client_socket);
        exit(4);
    }

    char response[MAX_RESPONSE_SIZE];

    //send code to server G
    if(recv(client_socket, response, MAX_RESPONSE_SIZE,0) == -1) {
        perror("recv");
        close(client_socket);
        exit(5);
    }

    printf("response: %s\n", response);

    close(client_socket);
    return 0;
}

