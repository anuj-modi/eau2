#pragma once
#include <unordered_map>

#include "key.h"
#include "network/network_ifc.h"
#include "util/lock.h"
#include "value.h"

/**
 * Key value store.
 * Author: gomes.chri, modi.an
 */
class KVStore : public Object {
   public:
    std::unordered_map<Key, Value*> items_;
    NetworkIfc* net_;
    Lock l_;

    KVStore() : Object() {
        items_ = std::unordered_map<Key, Value*>();
        net_ = nullptr;
    }

    KVStore(NetworkIfc* net) : Object() {
        assert(net != nullptr);
        items_ = std::unordered_map<Key, Value*>();
        net_ = net;
    }

    virtual ~KVStore() {
        for (std::unordered_map<Key, Value*>::iterator it = items_.begin(); it != items_.end();
             it++) {
            delete it->second;
        }
    }

    /**
     * Checks if the given key is in the kvstore. Local helper
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in_(Key& k) {
        return items_.find(k) != items_.end();
    }

    /**
     * Gets the value at the given key.
     * Returns a copy of the value.
     * @arg k  the key
     * @return the value
     */
    virtual Value* get(Key& k) {
        if (k.node_ == this_node()) {
            l_.lock();
            assert(items_.find(k) != items_.end());
            Value* result = items_.at(k);
            l_.unlock();
            return result->clone();
        } else {
            assert(net_ != nullptr);
            return net_->get_from_node(k.node_, k);
        }
    }

    /**
     * Gets the number of nodes that the store is operating over.
     * @return number of nodes
     */
    virtual size_t num_nodes() {
        if (net_ != nullptr) {
            return net_->num_nodes();
        } else {
            return 1;
        }
    }

    /**
     * Get number of node that this instance is running on.
     * @return the node number
     */
    virtual size_t this_node() {
        if (net_ != nullptr) {
            return net_->this_node();
        } else {
            return 0;
        }
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    virtual Value* waitAndGet(Key& k) {
        if (k.node_ == this_node()) {
            l_.lock();
            while (items_.find(k) == items_.end()) {
                l_.wait();
            }
            Value* result = items_.at(k);
            l_.unlock();
            return result->clone();
        } else {
            assert(net_ != nullptr);
            return net_->wait_and_get_from_node(k.node_, k);
        }
    }

    /**
     * Puts the value at the given key.
     * Copies the Key and consumes the Value.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    virtual void put(Key& k, Value* v) {
        if (k.node_ == this_node()) {
            l_.lock();
            if (items_.find(k) != items_.end()) {
                delete items_[k];
                items_[k] = v;
            } else {
                items_[Key(k)] = v;
            }
            l_.notify_all();
            l_.unlock();
        } else {
            assert(net_ != nullptr);
            net_->put_at_node(k.node_, k, v);
        }
    }
};

// The following methods are only here because of really silly circular dependency issues.

inline void Connection::handle_get_message_(Deserializer& d) {
    Get g(&d);
    Reply r(local_store_->get(g.k_));
    send_message(&r);
}

inline void Connection::handle_put_message_(Deserializer& d) {
    Put p(&d);
    local_store_->put(p.k_, p.v_->clone());
}

inline void Connection::handle_wait_and_get_message_(Deserializer& d) {
    WaitAndGet g(&d);
    Reply r(local_store_->waitAndGet(g.k_));
    send_message(&r);
}
