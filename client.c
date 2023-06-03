#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#define BUFFER_SIZE 1024
#define MAX_ITER 3
int main(){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUFFER_SIZE];

    //socket
    portno = atoi("8080");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    server = gethostbyname("localhost");
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    //persistency check
    for(int i=0; i< MAX_ITER; i++){
        send(sockfd, "Hello server", 12, 0);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        printf("SERVER REPLY NO. %d: %s\n", i+1, buffer);
    }
    close(sockfd);
}