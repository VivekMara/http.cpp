#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void die(const char* msg){
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}



int main(){
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        die("socket()");
    }
    
    //socket options
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    //bind to a port
    struct sockaddr_in addr{
        addr.sin_family = AF_INET,
        addr.sin_port = ntohs(8080),
        addr.sin_addr.s_addr = ntohl(0),//this means that the socket will listen on all available network interfaces
    };
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("bind()");
    }
    //listen
    rv = listen(fd, SOMAXCONN); //SOMAXCONN is a macro that that specifies the maximum number of pending connections that can be queued on the socket.
    if (rv)
    {
        die("listen()");
    }
    
    while (true){
        //accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0)
        {
            continue; 
        }
        
        char rbuff[64] = {};
        ssize_t n = read(connfd, rbuff, sizeof(rbuff) - 1);
        if (n < 0)
        {
            fprintf(stderr, "read() error");
            return 0;
        }
        printf("client says: %s\n", rbuff);
        char wbuff[] = "world";
        write(connfd, wbuff, strlen(wbuff));
        close(connfd);
    }
    return 0;
}