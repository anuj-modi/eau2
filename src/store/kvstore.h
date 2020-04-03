#pragma once
#include <unordered_map>
#include "key.h"
#include "util/lock.h"
#include "value.h"

/**
 * Key value store.
 * Author: gomes.chri, modi.an
 */
class KVStore : public Object {
   public:
    std::unordered_map<Key, Value*> items_;
    Lock l_;

    KVStore() : Object() {
        items_ = std::unordered_map<Key, Value*>();
    }

    virtual ~KVStore() {
        for (std::unordered_map<Key, Value*>::iterator it = items_.begin(); it != items_.end();
             it++) {
            delete it->second;
        }
    }

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key& k) {
        l_.lock();
        bool result = items_.find(k) != items_.end();
        l_.unlock();
        return result;
    }

    /**
     * Gets the value at the given key.
     * Returns a copy of the value.
     * @arg k  the key
     * @return the value
     */
    virtual Value* get(Key& k) {
        l_.lock();
        assert(items_.find(k) != items_.end());
        Value* result = items_.at(k);
        l_.unlock();
        return result->clone();
    }

    /**
     * Gets the number of nodes that the store is operating over.
     * @return number of nodes
     */
    virtual size_t num_nodes() {
        return 1;
    }

    /**
     * Get number of node that this instance is running on.
     * @return the node number
     */
    virtual size_t this_node() {
        return 0;
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    virtual Value* waitAndGet(Key& k) {
        l_.lock();
        while (items_.find(k) == items_.end()) {
            l_.wait();
        }
        Value* result = items_.at(k);
        l_.unlock();
        return result->clone();
    }

    /**
     * Puts the value at the given key.
     * Copies the Key and consumes the Value.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    virtual void put(Key& k, Value* v) {
        l_.lock();
        if (items_.find(k) != items_.end()) {
            delete items_[k];
            items_[k] = v;
        } else {
            items_[Key(k)] = v;
        }
        l_.notify_all();
        l_.unlock();
    }
};
