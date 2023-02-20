//
// Created by Mattia Danisi on 19/02/23.
//

#ifndef GREEN_PASS_WRAPPER_H
#define GREEN_PASS_WRAPPER_H

#include <sys/socket.h>

ssize_t fullWrite(int, const void*, size_t);
ssize_t fullRead(int, const void*, size_t);


#endif //GREEN_PASS_WRAPPER_H
