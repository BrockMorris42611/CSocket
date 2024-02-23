#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <netdb.h> // Include netdb.h for getaddrinfo()
#include <errno.h>
//bryan and ohallarant pg 953
//edit c/cpp extension to have cmd argument "std=gnu17"
int main(){
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(0x50); //port 80 in hex (host -> network byte order)
    //                       localhost:127.0.0.1
    //                          7F=127|00=0|00=0|001=1
    saddr.sin_addr.s_addr = htonl(0x7F000001);//localhost in hex (host -> network byte order)
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1){
        perror("ERROR creating server fd");
        return EXIT_FAILURE;
    }
    int binder = bind(serverfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_in));
    if(binder == -1){
        perror("ERROR creating binder");
        return EXIT_FAILURE;
    }
    int listener = listen(serverfd, 1024);
    if(listener == -1){
        perror("ERROR creating listener");
        return EXIT_FAILURE;
    }
    int accepted = accept(serverfd, (struct sockaddr*) &saddr, &(socklen_t){sizeof(saddr)});

    if(accepted == -1){
        perror("ERROR creating accepted");
        return EXIT_FAILURE;
    }

    char hello[128];
    while(recv(accepted, hello, sizeof(hello), 0)){
        printf("from client> %s", hello);
    }
    return EXIT_SUCCESS;
}