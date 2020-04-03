#pragma once
#include <unordered_map>

#include "connection.h"
#include "network.h"
#include "store/kvstore.h"
#include "util/thread.h"

class NetworkIfc : public Thread {
   public:
    size_t node_num_;
    size_t total_nodes_;
    std::unordered_map<size_t, Connection*> connections_;
    ListenSocket* listen_sock_;
    KVStore* local_kv_;

    NetworkIfc(size_t node_num, size_t total_nodes, KVStore* kv)
        : Thread(), node_num_(node_num), total_nodes_(total_nodes), connections_() {
        local_kv_ = kv;
        listen_sock_ = new ListenSocket();
    }

    void put_at_node(size_t node, Key& k, Value* v);
    Value* get_from_node(size_t node, Key& k);
    Value* wait_and_get_from_node(size_t node, Key& k);
};
