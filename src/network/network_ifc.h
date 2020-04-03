#pragma once
#include <cassert>
#include <unordered_map>
// #include <vector>

#include "connection.h"
#include "message.h"
#include "network.h"
#include "store/kvstore.h"
#include "util/thread.h"

class NetworkIfc : public Thread {
   public:
    size_t node_num_;
    size_t total_nodes_;
    std::unordered_map<size_t, Connection*> connections_;
    std::unordered_map<size_t, Address*> peer_addresses_;
    ListenSocket* listen_sock_;
    KVStore* local_kv_;

    NetworkIfc(size_t node_num, size_t total_nodes, KVStore* kv)
        : Thread(), node_num_(node_num), total_nodes_(total_nodes), connections_() {
        local_kv_ = kv;
        listen_sock_ = new ListenSocket();
    }

    void connect_to_node_(size_t node) {
        if (connections_.find(node) == connections_.end()) {
            assert(peer_addresses_.size() == total_nodes_);
            ConnectionSocket* cs = new ConnectionSocket();
            cs->connect_to_other(peer_addresses_.at(node));
            Connection* c = new Connection(cs, local_kv_);
            c->start();
            connections_[node] = c;
        }
    }

    void put_at_node(size_t node, Key& k, Value* v) {
        assert(node_num_ != node);
        Put p(k, v);
        connect_to_node_(node);
        connections_.at(node)->send_message(&p);
    }

    Value* process_reply_(Connection* c) {
        // Wait for a reply to come back
        while (c->replies.size() == 0) {
            continue;
        }

        assert(c->replies.size() == 1);

        Reply* r = c->replies.front();
        Value* result = r->v_->clone();

        c->replies.pop();
        delete r;

        return result;
    }

    Value* get_from_node(size_t node, Key& k) {
        assert(node_num_ != node);
        Get g(k);

        connect_to_node_(node);
        Connection* c = connections_.at(node);
        assert(c->replies.size() == 0);
        c->send_message(&g);

        return process_reply_(c);
    }

    Value* wait_and_get_from_node(size_t node, Key& k) {
        assert(node_num_ != node);
        WaitAndGet g(k);

        connect_to_node_(node);
        Connection* c = connections_.at(node);
        assert(c->replies.size() == 0);
        c->send_message(&g);

        return process_reply_(c);
    }
};
