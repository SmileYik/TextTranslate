// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#include "rio.h"
#include <errno.h>
#include <unistd.h>

ssize_t Rio::rio_readn(int fd, void* usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nread;
    char* bufp = (char*) usrbuf;

    while (nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0;
            } else {
                return -1;
            }
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft;
}

ssize_t Rio::rio_writen(int fd, void* usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwrittend;
    char* bufp = (char*) usrbuf;

    while (nleft > 0) {
        if ((nwrittend = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) {
                nwrittend = 0;
            } else {
                return -1;
            }
        }
        nleft -= nwrittend;
        bufp += nwrittend;
    }
    return n;
}
