#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#define BUF_SIZE 1024

char resources_dir[BUF_SIZE];

void send_file(int new_sock, char* path) {
    char responce[BUF_SIZE];
    char file_path[BUF_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/%s", resources_dir, path);

    FILE* file = fopen(file_path, "rb");
    char buffer[BUF_SIZE];

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long int size = ftell(file);
        fseek(file , 0, SEEK_SET);

        if(!strcmp(path, "index.html")){
            sprintf(responce, "HTTP/1.1 200 OK\r\n"
                    "Connection: Keep-Alive\r\n"
                    "keep-alive: timeout=5, max=30\r\n"
                    "Content-length: %ld\r\n"
                    "Content-Type: text/html\r\n\r\n", size);
        }
        else if(!strcmp(path, "script.js")){
            sprintf(responce, "HTTP/1.1 200 OK\r\n"
                    "Connection: Keep-Alive\r\n"
                    "keep-alive: timeout=5, max=30\r\n"
                    "Content-length: %ld\r\n"
                    "Content-Type: text/js\r\n\r\n", size);
        }
        else if(!strcmp(path, "gr-small.png")){
            sprintf(responce, "HTTP/1.1 200 OK\r\n"
                    "Connection: Keep-Alive\r\n"
                    "keep-alive: timeout=5, max=30\r\n"
                    "Content-length: %ld\r\n"
                    "Content-Type: image/png\r\n\r\n", size);
        }
        else if(!strcmp(path, "gr-large.jpg")){
            sprintf(responce, "HTTP/1.1 200 OK\r\n"
                    "Connection: Keep-Alive\r\n"
                    "keep-alive: timeout=5, max=30\r\n"
                    "Content-length: %ld\r\n"
                    "Content-Type: image/jpg\r\n\r\n", size);
        }
        send(new_sock, responce, strlen(responce), 0);
        size_t n;
        while((n = fread(buffer, 1, BUF_SIZE, file)) > 0) {
            send(new_sock, buffer, n, 0);
        }
        fclose(file);
    } else {
        sprintf(responce, "HTTP/1.1 404 Not Found\r\n");
        send(new_sock, responce, strlen(responce), 0);
    }
}


void process_request(int client_socket, char* request){
    char method[5];
    char path[BUF_SIZE];
    char connection_status[BUF_SIZE];
    sscanf(request, "%s %s", method, path);

    if (strcmp(method, "GET") == 0) {
        if (path[0] == '/') {
            memmove(path, path + 1, strlen(path));
        }
        if (strlen(path) == 0) {
            strcpy(path, "index.html");
        }
        
        send_file(client_socket, path);
    } else {
        // Unsupported method
        char* response = "HTTP/1.1 400 Bad Request\r\n";
        send(client_socket, response, strlen(response), 0);
    }
}

int send_HTTP_responce(int client_socket, char* request, fd_set* current_sockets){
    if(!recv(client_socket, request, 1024, 0)){
        //close(client_socket);
        //FD_CLR(client_socket, current_sockets);
        return -1;
    }
    process_request(client_socket, request);
    return 0;
}

int main(int argc, char* argv[]){
    if(argc != 3){
        perror("undefined usage");
        exit(1);
    }
    char buffer[BUF_SIZE] = {0};
    int port = atoi(argv[1]);
    strncpy(resources_dir, argv[2], BUF_SIZE - 1);
    resources_dir[BUF_SIZE - 1] = '\0';

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
                    //process request
                    memset(buffer, 0, BUF_SIZE);
                    if(send_HTTP_responce(i, buffer, &current_sockets) < 0){
                        continue;
                    }
                }
            }
        }
    }
    close(server_sock);

}