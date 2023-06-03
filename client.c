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
    char* http_get_request = "GET / HTTP/1.1\r\n";
    char* http_post_request = "POST / HTTP/1.1\r\n";
    char* http_bad_file_request = "GET /hello.txt HTTP/1.1\r\n";
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
    memset(buffer, 0, BUFFER_SIZE);
    send(sockfd, http_get_request, strlen(http_get_request), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("SERVER REPLY NO. %d: %s\n", 1, buffer);
    
    memset(buffer, 0, BUFFER_SIZE);
    send(sockfd, http_post_request, strlen(http_post_request), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("SERVER REPLY NO. %d: %s\n", 2, buffer);

    memset(buffer, 0, BUFFER_SIZE);
    send(sockfd, http_bad_file_request, strlen(http_bad_file_request), 0);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("SERVER REPLY NO. %d: %s\n", 3, buffer);
    close(sockfd);
}