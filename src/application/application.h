#pragma once
#include <assert.h>

#include "store/kdstore.h"
#include "store/kvstore.h"
#include "util/thread.h"

/**
 * Application that runs on eau2.
 * Author: gomes.chri and modi.an
 */
class Application : public Thread {
   public:
    size_t node_num_;
    KVStore* kv_;
    KDStore kd;

    Application(size_t node_num, KVStore* kv) : Thread(), node_num_(node_num), kv_(kv), kd(kv_) {}

    virtual ~Application() {}

    /**
     * Starts the applciation thread.
     */
    virtual void run() {
        assert(false);
    }

    /**
     * Gives the node that this instance is running on.
     * @return the node number
     */
    size_t this_node() {
        return node_num_;
    }
};
