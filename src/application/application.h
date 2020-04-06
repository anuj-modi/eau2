#pragma once
#include <assert.h>

#include "network/network_ifc.h"
#include "store/kdstore.h"
#include "store/kvstore.h"
#include "util/thread.h"

/**
 * Application that runs on eau2.
 * Author: gomes.chri and modi.an
 */
class Application : public Thread {
   public:
    NetworkIfc& net_;
    KVStore kv_;
    size_t node_num_;
    KDStore kd_;

    Application(NetworkIfc& net)
        : Thread(), net_(net), kv_(&net_), node_num_(net_.this_node()), kd_(&kv_) {
        net_.set_kv(&kv_);
    }

    virtual ~Application() {
        net_.stop();
        net_.join();
    }

    virtual void start() override {
        net_.start();
        Thread::start();
    }

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
