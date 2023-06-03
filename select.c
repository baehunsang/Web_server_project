#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

int make_HTTP_responce(int client_socket, char* request, char* responce, fd_set* current_sockets){
    if(!recv(client_socket, request, 1024, 0)){
        close(client_socket);
        FD_CLR(client_socket, current_sockets);
        return -1;
    }
    printf("Client: %s\n", request);
    send(client_socket, responce, 1024, 0);
    return 0;
}

int main(){
    char message[1024] = "hello\n";
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

    //client socket info
    int client_socket;
    int max_socket;
    struct sockaddr_in client_addr;
    socklen_t clilen = sizeof(client_addr);

    fd_set current_sockets, tmp_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(server_sock, &current_sockets);
    max_socket = server_sock;

    while(1){
        tmp_sockets = current_sockets;
        select(max_socket + 1, &tmp_sockets, NULL, NULL, NULL);

        for(int i=0; i <= max_socket; i++){
            if(FD_ISSET(i, &tmp_sockets)){
                //handle new connection
                if(i == server_sock){
                    client_socket = accept(server_sock, (struct sockaddr*)&client_addr, &clilen);
                    FD_SET(client_socket, &current_sockets);
                    if(client_socket > max_socket){
                        max_socket = client_socket;
                    }
                }
                else{
                    //process request{
                    if(make_HTTP_responce(i, buffer, message, &current_sockets) < 0){
                        continue;
                    }
                }
            }
        }
    }
    close(server_sock);

}