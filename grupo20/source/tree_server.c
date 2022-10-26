#include <stdlib.h>
#include <stdio.h>
#include "../include/tree_skel.h"
#include "../include/network_server.h"

int main(int argc, char* argv[]) {
    int socketfd;

    if((socketfd = network_server_init(atoi(argv[1]))) == -1)
        return -1;

    if(tree_skel_init() == -1)
        return -1;

    if(network_main_loop(socketfd) == -1)
        return -1;

    tree_skel_destroy();

    return network_server_close();
}