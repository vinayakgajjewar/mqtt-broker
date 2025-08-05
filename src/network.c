#define _DEFAULT_SOURCE

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#include "network.h"

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("set_nonblocking");
        return -1;
    }
    int result = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (result == -1) {
        perror("set_nonblocking");
        return -1;
    }
    return 0;
}

int set_tcp_nodelay(int fd) {
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &(int) {1}, sizeof(int));
}

static int create_and_bind_unix(const char *sockpath) {
    struct sockaddr_un addr;
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket error");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sockpath, sizeof(addr.sun_path) - 1);
    unlink(sockpath);
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind error");
        return -1;
    }
    return fd;
}

static int create_and_bind_tcp(const char *host, const char *port) {
    struct addrinfo hints = {.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM, .ai_flags = AI_PASSIVE};
    struct addrinfo *result;
    if (getaddrinfo(host, port, &hints, &result) != 0) {
        perror("getaddrinfo error");
        return -1;
    }
    struct addrinfo *rp;
    int sfd;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) continue;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int)) < 0) {
            perror("SO_REUSEADDR");
        }
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(sfd);
    }
    if (rp == NULL) {
        perror("could not bind");
    }
    freeaddrinfo(result);
    return sfd;
}

int create_and_bind(const char *host, const char *port, int socket_family) {
    int fd;
    if (socket_family == UNIX) {
        fd = create_and_bind_unix(host);
    } else {
        fd = create_and_bind_tcp(host, port);
    }
    return fd;
}

int make_listen(const char *host, const char *port, int socket_family) {
    int sfd = create_and_bind(host, port, socket_family);
    if (sfd == -1) {
        abort();
    }
    if (set_nonblocking(sfd) == -1) {
        abort();
    }
    if (socket_family == INET) {
        set_tcp_nodelay(sfd);
    }

    /*
     * Queue up at most 10 connections at a time.
     */
    int backlog = 10;
    if (listen(sfd, backlog) == -1) {
        perror("listen");
        abort();
    }
}

int accept_connection(int serversock) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int clientsock = accept(serversock, (struct sockaddr *) &addr, &addrlen);
    if (clientsock < 0) {
        return -1;
    }
    set_nonblocking(clientsock);

    /*
     * Set TCP_NODELAY for TCP sockets.
     */
    int socket_family = INET;
    if (socket_family == INET) {
        set_tcp_nodelay(clientsock);
    }
    char ip_buff[INET_ADDRSTRLEN + 1];
    if (inet_ntop(AF_INET, &addr.sin_addr, ip_buff, sizeof(ip_buff)) == NULL) {
        close(clientsock);
        return -1;
    }
    return clientsock;
}

// TODO: send_bytes() and recv_bytes()