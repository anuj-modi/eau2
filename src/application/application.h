#pragma once
#include <assert.h>

#include "network/network_ifc.h"
#include "store/kdstore.h"
#include "store/kvstore.h"
#include "util/thread.h"

/**
 * Application that runs on eau2.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
class Application : public Thread {
   public:
    NetworkIfc& net_;
    KVStore kv_;
    size_t node_num_;
    KDStore kd_;

    /**
     * Constructs an Application object using a provided network interface.
     * @arg the network layer interface for this application to use
     */
    Application(NetworkIfc& net)
        : Thread(), net_(net), kv_(&net_), node_num_(net_.this_node()), kd_(&kv_) {
        net_.set_kv(&kv_);
    }

    /**
     * Destructs an application object.
     */
    virtual ~Application() {
        net_.stop();
        net_.join();
    }

    /**
     * Starts this application by starting the network and the underlying
     * thread.
     */
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
        return net_.this_node();
    }

    /**
     * Returns the number of nodes in this application
     * @return the number of nodes
     */
    size_t num_nodes() {
        return net_.num_nodes();
    }
};
