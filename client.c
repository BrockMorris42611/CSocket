#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*for sockets*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

/*for threading*/
#include <sys/fcntl.h>
#include <pthread.h>

//bryan and ohallarant pg 953
int main(){
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(0x50); //port 80 in hex (in network byte order)
    //                      localhost:127.0.0.1
    //                          7F=127|00=0|00=0|001=1
    saddr.sin_addr.s_addr = htonl(0x7F000001);
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1){
        perror("ERROR creating server fd");
        return EXIT_FAILURE;
    }
    printf("sock: %d", clientfd);
    int conn = connect(clientfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if(conn == -1){
        perror("ERROR creating listener");
        return EXIT_FAILURE;
    }
    char buffy[128];
    while(fgets(buffy, sizeof(buffy), stdin) != NULL){
        if(send(clientfd, buffy, strlen(buffy), 0)==-1){
            close(clientfd);
            perror("Something went wrong when send()");
        }
    }
    close(clientfd);
    return EXIT_SUCCESS;
}