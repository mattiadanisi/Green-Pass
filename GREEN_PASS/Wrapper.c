//
// Created by Mattia Danisi on 19/02/23.
//

#include "Wrapper.h"

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

ssize_t fullWrite(int fd, const void *buf, size_t count) {
    size_t nleft;
    size_t nwritten;
    nleft = count;

    while (nleft > 0) {
        if((nwritten = write(fd, buf, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                exit(nwritten);
            }
        }
        nleft -= nwritten;
        buf += nwritten;
    }
    return (nleft);
}

ssize_t fullRead(int fd, const void *buf, size_t count) {
    size_t nleft;
    size_t nread;
    nleft = count;

    while (nleft > 0) {
        if((nread = read(fd, buf, nleft)) < 0) {
            if (errno == EINTR) {
                continue;
            } else {
                exit(nread);
            }
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        buf += nread;
    }
    buf = 0;
    return (nleft);
}


