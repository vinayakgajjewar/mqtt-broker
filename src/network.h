#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include "util.h"

#define UNIX 0
#define INET 1

/*
 * Set a socket to non-blocking mode.
 */
int set_nonblocking(int);

/*
 * Disable Nagle's algorithm.
 */
int set_tcp_nodelay(int);

int create_and_bind(const char *, const char *, int);

/*
 * Create a non-blocking socket and make it listen on the specified address and
 * port.
 */
int make_listen(const char *, const char *, int);

/*
 * Accept a connection.
 */
int accept_connection(int);

/*
 * Send an arbitrary number of bytes across a file descriptor, regardless of
 * kernel buffer availability.
 */
ssize_t send_bytes(int, const unsigned char *, size_t);

/*
 * Receive an arbitrary number of bytes across a file descriptor, regardless of
 * kernel buffer availability.
 */
ssize_t recv_bytes(int, unsigned char *, size_t);

#endif