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
#define NUM_OF_THREADS 2
/*---------------------------------------------------------------------*/
void *begin_talking(void *__fd);
int hasNullTerm(char *s);

//bryan and ohallarant pg 953
//edit c/cpp extension to have cmd argument "-std=gnu17"

int main(){
    int server_fd, binder, listener, accepted;

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(0x50), //Port 80 in hex (host->network byte order)
        .sin_addr.s_addr = htonl(0x7F000001) //Localhost: 127.0.0.1 in hex (host->network byte order)
    };
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){perror("ERROR creating server fd");
        return EXIT_FAILURE;
    }
    binder = bind(server_fd, (struct sockaddr*) &saddr, sizeof(struct sockaddr_in));
    if(binder == -1){perror("ERROR creating binder");
        return EXIT_FAILURE;
    }
    listener = listen(server_fd, 1024);
    if(listener == -1){perror("ERROR creating listener");
        return EXIT_FAILURE;
    }
    while(1){
        accepted = accept(server_fd, (struct sockaddr*) &saddr, &(socklen_t){sizeof(saddr)});
        if(accepted == -1){perror("ERROR creating accepted");
            return EXIT_FAILURE;
        }
        printf("sock: %d\ninstance:%d\n", server_fd, accepted);
        pthread_t new_conn_pt; pthread_attr_t new_conn_attr;
        pthread_attr_init(&new_conn_attr);
        pthread_attr_setstacksize(&new_conn_attr, 200);
        pthread_create(&new_conn_pt, &new_conn_attr, begin_talking, &accepted);
    }
    close(server_fd);
    return EXIT_SUCCESS;
}
void *begin_talking(void *fd){
    pthread_detach(pthread_self());
    u_int8_t active_sock_fd = *(u_int8_t*)fd;
    char hello[128]; ssize_t bytes_recv;
    memset(hello, 0, sizeof(hello));
    while((bytes_recv = recv(active_sock_fd, hello, sizeof(hello), 0)) > 0){
        printf("%ld bytes from client> %s",bytes_recv, hello);\
        memset(hello, 0, sizeof(hello));
    }
    if (bytes_recv == -1) {
        perror("ERROR receiving data");
        close(active_sock_fd); // Close the accepted socket on error
        pthread_exit((void*)EXIT_FAILURE);
    }
    close(active_sock_fd);//Close the accepted socket after the loop
    pthread_exit((void*)EXIT_SUCCESS);
}