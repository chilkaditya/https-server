#include <iostream>
#include <winsock2.h>

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        die("WSAStartup() failed");
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("connect");
    }

    // char msg[] = "hello";
    std::string msg;
    std::cin >> msg;
    send(fd, msg.c_str(), msg.size(),0);

    char rbuf[64] = {};
    ssize_t n = recv(fd, rbuf, sizeof(rbuf) - 1,0);
    if (n < 0) {
        die("read");
    }
    printf("server says: %s\n", rbuf);
    // Cleanup Winsock
    closesocket(fd);
    WSACleanup();
    return 0;
}