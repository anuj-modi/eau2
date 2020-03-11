#include "server.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>

char* ip;
Server* s;

void shutdown_server(int signum) {
    s->shutdown();
    printf("\nShutting down...\n");
    delete s;
    exit(0);
}

int main(int argc, char** argv) {
    if (argc != 3 || strcmp(argv[1], "-ip") != 0) {
        printf("please provide an IP\n");
        exit(1);
    }

    signal(SIGINT, shutdown_server);

    ip = argv[2];
    s = new Server(ip, 4444);
    s->run();
    return 0;
}
