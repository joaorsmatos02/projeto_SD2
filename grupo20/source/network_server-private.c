#include <errno.h>
#include "../include/network_server-private.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int read_all(int sock, const uint8_t* buf, int len) {
    int bufsize = len;
    void* buffer = (void*) buf;

    while(len > 0) {
        int res = read(sock, buffer, len);
        if(res <= 0) {
            if(errno==EINTR)
                continue;
            perror("read failed");
            return res;
        }
        buffer += res;
        len -= res;
    }

    buf = (const uint8_t*) buffer;

    return bufsize;
}

int write_all(int sock, const uint8_t* buf, int len) {
    int bufsize = len;
    void* buffer = (void*) buf;

    while(len > 0) {
        int res = write(sock, buffer, len);
        if(res <= 0) {
            if(errno==EINTR)
                continue;
            perror("write failed");
            return res;
        }
        buffer += res;
        len -= res;
    }

    return bufsize;
}