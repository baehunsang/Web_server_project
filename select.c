#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main(){
    char message[1024] = "hello client\n";
    char buffer[1024] = {0};
    int port = 8080;

    //initialize server data
    int server_sock;
    struct sockaddr_in serv_addr;
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    //socket
    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket error");
        exit(1);
    }

    if (bind(server_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("binding error");
        exit(1);
    }

    listen(server_sock, 5);

    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t clilen = sizeof(client_addr);

    while(1){
        client_socket = accept(server_sock, &client_addr, &clilen);

        if (client_socket < 0) {
                continue;
        }

        //process request
        recv(client_socket, buffer, 1024, 0);
        printf("Client: %s\n", buffer);
        send(client_socket, message, 1024, 0);
        close(client_socket);
    }
    close(server_sock);

}