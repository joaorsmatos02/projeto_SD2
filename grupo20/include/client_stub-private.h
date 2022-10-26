#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

struct rtree_t {
    char* ip_addr;
    char* port;
    int sockfd;
};

#endif