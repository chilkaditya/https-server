#include <iostream>
#include <winsock2.h>

static void msg(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static void do_something(int connfd) {
    char rbuf[4096] = {};
    ssize_t n = recv(connfd, rbuf, sizeof(rbuf) - 1,0);
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    send(connfd, wbuf, strlen(wbuf),0);
}

int main(){
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        die("WSAStartup() failed");
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << fd << std::endl;
    if (fd < 0) {
        die("socket()");
    }
    
    //bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);    // wildcard address 0.0.0.0
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("bind()");
    }

    // listen
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        die("listen()");
    }

    while (true) {
        // accept
        struct sockaddr_in client_addr = {};
        int socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0) {
            continue;   // error
        }

        do_something(connfd);
        closesocket(connfd);
    }
    // Cleanup Winsock
    closesocket(fd);
    WSACleanup();
}