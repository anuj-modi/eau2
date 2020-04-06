#include <stdio.h>
#include <string.h>

#include "network.h"
#include "network_ifc.h"
#include "store/kvstore.h"

int main(int argc, char** argv) {
    // if (argc != 3 || strcmp(argv[1], "-ip") != 0) {
    //     printf("please provide an IP");
    //     exit(1);
    // }

    // char* ip = argv[2];
    // const char* server_ip = "127.0.0.1";

    Address server("127.0.0.1", 5555);
    Address me("127.0.0.1", 4444);
    KVStore kv;
    NetworkIfc ifc(&me, &server, 1, 2, &kv);

    ifc.start();

    ifc.join();
    return 0;
}
