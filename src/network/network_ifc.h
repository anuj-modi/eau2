#include <unordered_map>
#include <vector>

#include "message.h"
#include "network.h"
#include "store/key.h"
#include "store/kvstore.h"
#include "store/value.h"
#include "util/object.h"
#include "util/serial.h"
#include "util/thread.h"

class Node : public Object {
   public:
    ListenSocket* listen_sock_;
    std::vector<ConnectionSocket*> peer_sockets_;
    std::vector<Address*> peer_addresses_;
    Address* my_addr_;

    Node(const char* ip, uint16_t port) : Object(), peer_sockets_(), peer_addresses_() {}
};

class Connection : public Thread {
   public:
    ConnectionSocket* s_;
    KVStore* local_store_;
    bool keep_processing_;

    Connection(ConnectionSocket* s, KVStore* kv) : Thread() {
        s_ = s;
        local_store_ = kv;
        keep_processing_ = true;
    }

    void send_message_(Message* m) {
        Serializer s;
        m->serialize(&s);
        size_t num_bytes = s.size();
        assert(s_->send_bytes((char*)&num_bytes, sizeof(size_t)) > 0);
        assert(s_->send_bytes(s.get_bytes(), num_bytes) > 0);
    }

    Deserializer recv_message_() {
        size_t num_bytes = 0;
        assert(s_->recv_bytes((char*)&num_bytes, sizeof(size_t)) > 0 && num_bytes > 0);
        char* buf = new char[num_bytes];
        assert(s_->recv_bytes(buf, num_bytes) > 0);
        return Deserializer(buf, num_bytes);
    }

    /**
     * Processes the bytes received from the socket.
     */
    void handle_status_message_(Deserializer& d) {
        Status s(&d);
        printf("%s\n", s.message_->c_str());
    }

    void handle_get_message_(Deserializer& d) {
        Get g(&d);
        Reply r(local_store_->get(g.k_));
        send_message_(&r);
    }

    void handle_put_message_(Deserializer& d) {
        Put p(&d);
        local_store_->put(p.k_, p.v_->clone());
    }

    void handle_wait_and_get_message_(Deserializer& d) {
        WaitAndGet g(&d);
        Reply r(local_store_->waitAndGet(g.k_));
        send_message_(&r);
    }

    void handle_message_(Deserializer& d) {
        MsgType m = d.get_msg_type();
        if (m == MsgType::STATUS) {
            handle_status_message_(d);
        } else if (m == MsgType::GET) {
            handle_get_message_(d);
        } else if (m == MsgType::PUT) {
            handle_put_message_(d);
        } else if (m == MsgType::WAITANDGET) {
            handle_wait_and_get_message_(d);
        }

        else {
            assert(false);
        }
    }

    // handles messages from other nodes coming in
    void run() override {
        while (keep_processing_) {
            if (s_->has_new_bytes()) {
                Deserializer d = recv_message_();
                handle_message_(d);
            }
        }
    }
};

class NetworkIfc : public Object {
   public:
    size_t node_num_;
    size_t total_nodes_;
    std::unordered_map<size_t, Connection*> connections_;
    KVStore* local_kv_;

    NetworkIfc(size_t node_num, size_t total_nodes, KVStore* kv)
        : Object(), node_num_(node_num), total_nodes_(total_nodes), connections_() {
        local_kv_ = kv;
    }

    void put_at_node(size_t node, Key& k, Value* v);
    Value* get_from_node(size_t node, Key& k);
    Value* wait_and_get_from_node(size_t node, Key& k);
};
