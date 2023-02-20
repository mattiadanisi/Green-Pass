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

    // Connect to the server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(CV_PORT);
    server_addr.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_socket);
        exit(3);
    }


    if(send(client_socket, argv[1], TESSERA_SANITARIA_LEN,0) == -1) {
        perror("send");
        close(client_socket);
        exit(4);
    }

    printf("code sent\n");

    close(client_socket);
    return 0;
}

