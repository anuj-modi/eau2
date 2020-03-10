#include "array.h"
#include "message.h"
#include "network.h"
#include "object.h"
#include "serial.h"
#include "string.h"

/**
 * Represents a client that connects and registers to a
 * server (controller).
 */
class Client : public Object {
   public:
    Socket* listen_sock_;
    uint16_t listen_port_;
    Socket* server_connection_;
    Array* peer_sockets_;
    Array* peer_addresses_;
    Address* my_addr_;

    /**
     * Creates an instance of client.
     * @arg ip  the ip to listen on
     * @arg port  port to listen on
     * @arg server_ip  the server ip to bind on
     * @arg server_port  the server port to bind on
     */
    Client(const char* ip, uint16_t port, const char* server_ip, uint16_t server_port) {
        peer_sockets_ = new Array();
        peer_addresses_ = new Array();
        listen_sock_ = new Socket();
        listen_sock_->bind_and_listen(ip, port);
        listen_port_ = port;
        my_addr_ = new Address(ip);
        server_connection_ = new Socket();
        server_connection_->connect_to_other(server_ip, server_port);
        Register* reg = new Register(new Address(my_addr_->as_bytes()));
        Serializer* s = new Serializer();
        reg->serialize(s);
        assert(server_connection_->send_bytes(s->get_bytes(), s->size()) > 0);
        delete s;
        delete reg;
    }

    /**
     * Deconstructs an instance of client.
     */
    virtual ~Client() {
        for (size_t i = 0; i < peer_sockets_->size(); i++) {
            delete peer_sockets_->get(i);
        }
        delete peer_sockets_;

        if (peer_addresses_ != nullptr) {
            delete peer_addresses_;
        }
        delete my_addr_;
        delete listen_sock_;
        delete server_connection_;
    }

    /**
     * Stores the addresses given by the server.
     */
    void handle_directory_message(Deserializer* d) {
        // clear my held addresses
        peer_addresses_->clear();

        // create Address array
        Directory* dir = new Directory(d);
        Address* a;
        for (size_t i = 0; i < dir->client_addrs_->size(); i++) {
            a = static_cast<Address*>(dir->client_addrs_->get(i));
            if (!my_addr_->equals(a)) {
                peer_addresses_->push_back(new Address(a->as_bytes()));
            }
            delete a;
        }
        delete dir->client_addrs_;
        delete dir;
    }

    /**
     * Creates a connection specific socket from the listening socket.
     */
    void handle_new_connections() {
        peer_sockets_->push_back(listen_sock_->accept_connection());
    }

    /**
     * Processes the bytes received from the socket.
     */
    void handle_status_message(Deserializer* d) {
        Status* s = new Status(d);
        printf("%s\n", s->message_->c_str());
        delete s;
    }

    /**
     * Sends a message to a peer at a given socket.
     */
    void send_status_message_(const char* msg, Socket* peer) {
        Status* stat = new Status(msg);
        Serializer* s = new Serializer();
        stat->serialize(s);
        size_t num_sent = peer->send_bytes(s->get_bytes(), s->size());
        assert(num_sent > 0);
        delete s;
        delete stat;
    };

    /**
     * Starts the processing loop for this client.
     */
    void run() {
        size_t num_server_messages = 0;
        bool sender = false;
        while (true) {
            if (server_connection_->has_new_bytes()) {
                char buf[1024];
                num_server_messages += 1;
                size_t num_bytes = server_connection_->recv_bytes(buf, sizeof(buf));
                assert(num_bytes > 0);
                Deserializer* d = new Deserializer(buf, num_bytes);
                switch (d->get_msg_type()) {
                    case MsgType::DIRECTORY:
                        handle_directory_message(d);
                        delete d;
                        break;
                    case MsgType::KILL:
                        printf("Shutting down...\n");
                        delete d;
                        return;
                    default:
                        assert(false);
                }
            }

            if (listen_sock_->has_new_connections()) {
                // process new connections
                handle_new_connections();
            }

            for (size_t i = 0; i < peer_sockets_->size(); i++) {
                Socket* peer = static_cast<Socket*>(peer_sockets_->get(i));
                if (peer->has_new_bytes()) {
                    char buf[1024];
                    size_t num_bytes = peer->recv_bytes(buf, sizeof(buf));
                    assert(num_bytes > 0);
                    Deserializer* d = new Deserializer(buf, num_bytes);
                    switch (d->get_msg_type()) {
                        case MsgType::STATUS:
                            handle_status_message(d);
                            break;
                        default:
                            assert(false);
                    }
                    if (!sender) {
                        // send a response
                        send_status_message_("hello back", peer);
                    }
                    return;
                }
            }

            if (!sender && num_server_messages == 2 && peer_addresses_->size() == 1) {
                sender = true;
                // connect to the other client
                Address* peer = static_cast<Address*>(peer_addresses_->get(0));
                Socket* peer_sock = new Socket();
                peer_sock->connect_to_other(peer->as_str()->c_str(), listen_port_);
                peer_sockets_->push_back(peer_sock);

                // send a message to the other client
                send_status_message_("hello", peer_sock);
            }
        }
    }
};
