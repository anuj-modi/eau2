// #pragma once
// #include "message.h"
// #include "network.h"
// #include "util/array.h"
// #include "util/object.h"
// #include "util/serial.h"
// #include "util/string.h"

// /**
//  * Represents a server (controller) that clients register with.
//  */
// class Server : public Object {
//    public:
//     Socket* server_sock_;
//     Array* clients_;
//     Array* addresses_;

//     /**
//      * Creates an instance of a server.
//      * @arg ip  the ip to listen on
//      * @arg port  the port to listen on
//      */
//     Server(const char* ip, uint16_t port) {
//         clients_ = new Array();
//         addresses_ = new Array();
//         server_sock_ = new Socket();
//         server_sock_->bind_and_listen(ip, port);
//     }

//     /**
//      * Deconstructs an instance of a server.
//      */
//     virtual ~Server() {
//         for (size_t i = 0; i < clients_->size(); i++) {
//             delete clients_->get(i);
//         }
//         delete clients_;

//         if (addresses_ != nullptr) {
//             for (size_t i = 0; i < addresses_->size(); i++) {
//                 delete addresses_->get(i);
//             }
//             delete addresses_;
//         }

//         delete server_sock_;
//     }

//     /**
//      * Sends message to all nodes.
//      */
//     void broadcast_(Message* message) {
//         Serializer* s = new Serializer();
//         message->serialize(s);
//         size_t num_bytes = s->size();
//         for (size_t i = 0; i < clients_->size(); i++) {
//             Socket* sock = static_cast<Socket*>(clients_->get(i));
//             // send all client addresses over server
//             assert(sock->send_bytes(s->get_bytes(), num_bytes) == num_bytes);
//         }
//         delete s;
//     }

//     /**
//      * Process a REGISTER message.
//      */
//     void handle_register_message_(Socket* client, Deserializer* d) {
//         Register* reg = new Register(d);
//         printf("Registered: %s\n", reg->client_addr_->as_str()->c_str());
//         addresses_->push_back(new Address(reg->client_addr_->as_bytes()));
//         clients_->push_back(client);
//         Directory* dir = new Directory(addresses_);
//         broadcast_(dir);
//         delete reg;
//         delete dir;
//     }

//     /**
//      * Shuts down server.
//      */
//     void shutdown() {
//         Message* kill_msg = new Message(MsgType::KILL);
//         broadcast_(kill_msg);
//         delete kill_msg;
//         return;
//     }

//     /**
//      * Startes the processing loop for the server.
//      */
//     void run() {
//         while (true) {
//             if (!server_sock_->has_new_connections()) {
//                 continue;
//             }

//             Socket* new_client = server_sock_->accept_connection();
//             char buf[1024];
//             size_t num_bytes = new_client->recv_bytes(buf, sizeof(buf));
//             assert(num_bytes > 0);
//             Deserializer* d = new Deserializer(buf, num_bytes);

//             // check message type
//             switch (d->get_msg_type()) {
//                 case MsgType::REGISTER:
//                     handle_register_message_(new_client, d);
//                     break;
//                 default:
//                     assert(false);
//             }
//             delete d;
//         }
//     }
// };
