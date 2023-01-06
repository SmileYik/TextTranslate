// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef RIO_H
#define RIO_H

#include "based_type.h"

#define RIO_BUFFER_SIZE 8192

/**
 * rio.
 */
class Rio {
public:
    /**
     * 无缓冲读.
     */
    static ssize_t rio_readn(int fd, void* usrbuf, size_t n);
    /**
     * 无缓冲写.
     */
    static ssize_t rio_writen(int fd, void* usrbuf, size_t n);
};

#endif // RIO_H
