#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*for sockets*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> // Include netdb.h for getaddrinfo()

/*for threading*/
#include <sys/fcntl.h>
#include <pthread.h>

//bryan and ohallarant pg 953
//edit c/cpp extension to have cmd argument "-std=gnu17"
void *begin_talking(void *__fd);
int NUM_OF_THREADS = 2;

int main(){
    pthread_t talking_thread;
    pthread_t talking_thread0;
    //int file_desc_arr[2];
    int serverfd, binder, listener, accepted, accepted0;

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(0x50), //Port 80 in hex (host->network byte order)
        .sin_addr.s_addr = htonl(0x7F000001) //Localhost: 127.0.0.1 in hex (host->network byte order)
    };
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1){perror("ERROR creating server fd");
        return EXIT_FAILURE;
    }
    binder = bind(serverfd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_in));
    if(binder == -1){perror("ERROR creating binder");
        return EXIT_FAILURE;
    }
    listener = listen(serverfd, 1024);
    if(listener == -1){perror("ERROR creating listener");
        return EXIT_FAILURE;
    }
    int i = 0;
    while(1){
        if(!i){
            accepted = accept(serverfd, (struct sockaddr*) &saddr, &(socklen_t){sizeof(saddr)});
            if(accepted == -1){perror("ERROR creating accepted");
                return EXIT_FAILURE;
            }
            printf("-sock: %d\n instance:%d\n", serverfd, accepted);
            pthread_create(&talking_thread, NULL, begin_talking, &accepted);
        }else{
            accepted0 = accept(serverfd, (struct sockaddr*) &saddr, &(socklen_t){sizeof(saddr)});
            if(accepted0 == -1){perror("ERROR creating accepted");
                return EXIT_FAILURE;
            }
            printf("*sock: %d\n instance:%d\n", serverfd, accepted0);
            pthread_create(&talking_thread0, NULL, begin_talking, &accepted0);
        }
    }
    
    pthread_join(talking_thread, NULL);
    pthread_join(talking_thread0, NULL);
    close(accepted); // Close the accepted socket after the loop
    return EXIT_SUCCESS;
}
void *begin_talking(void *fd){
    int sock_fd = *(int*)fd;
    char hello[128]; ssize_t bytes_recv;
    while((bytes_recv = recv(sock_fd, hello, sizeof(hello), 0)) > 0){
        printf("from client> %s", hello);
    }
    if (bytes_recv == -1) {
        perror("ERROR receiving data");
        close(sock_fd); // Close the accepted socket on error
        return (void *)EXIT_FAILURE;
    }
    return (void *)EXIT_SUCCESS;
}