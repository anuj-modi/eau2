#pragma once
#include <assert.h>
#include "store/kdstore.h"
#include "store/kvstore.h"
#include "util/thread.h"

class Application : public Thread {
   public:
    size_t node_num_;
    KVStore* kv_;
    KDStore kd;

    Application(size_t node_num, KVStore* kv) : Thread(), node_num_(node_num), kv_(kv), kd(kv_) {}

    virtual ~Application() {}

    virtual void run() {
        assert(false);
    }

    size_t this_node() {
        return node_num_;
    }
};
