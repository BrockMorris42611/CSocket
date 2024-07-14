#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*for sockets*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h> // For inet_ntoa
/*for threading*/
#include <sys/fcntl.h>
#include <pthread.h>

int gai_func();
//bryan and ohallarant pg 953
int main(){
    //         localhost:127.0.0.1
    //         7F=127|00=0|00=0|001=1 -> 0x7F000001
    int host = htonl(0x7F000001);
    int port = htons(0x50); //port 80 in hex (in network byte order)

    gai_func(host, port);

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = port; 
    saddr.sin_addr.s_addr = host;
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1){
        perror("ERROR creating server fd");
        return EXIT_FAILURE;
    }
    printf("sock: %d\n", clientfd);
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
int gai_func(){
    char *host = "127.0.0.1";
    char *port = "80";
    struct addrinfo *ret;
    struct addrinfo hints;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int gai_code = getaddrinfo(host, port, &hints, &ret);
    if(gai_code != 0){
        perror("ERROR with gai()");
        return EXIT_FAILURE;
    }
    struct sockaddr_in *ipv4_addr;
    char ip_str[INET_ADDRSTRLEN];
    // in_port_t port_num;
    while (ret != NULL) {
        ipv4_addr = (struct sockaddr_in *)ret->ai_addr;
        inet_ntop(AF_INET, &(ipv4_addr->sin_addr), ip_str, INET_ADDRSTRLEN);
        printf("IPv4 Address: %s\nPort #%d\n", ip_str, ntohs(ipv4_addr->sin_port));
        ret = ret->ai_next;
    }
    freeaddrinfo(ret); // Free the dynamically allocated memory
    printf("Exiting gai_func()\n");
    return EXIT_SUCCESS;
}