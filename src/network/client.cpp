// #include "client.h"
// #include <stdio.h>
// #include <string.h>

// int main(int argc, char** argv) {
//     if (argc != 3 || strcmp(argv[1], "-ip") != 0) {
//         printf("please provide an IP");
//         exit(1);
//     }

//     char* ip = argv[2];
//     const char* server_ip = "127.0.0.10";

//     Client* c = new Client(ip, 4444, server_ip, 4444);
//     c->run();
//     delete c;
//     return 0;
// }
