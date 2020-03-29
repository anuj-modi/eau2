#pragma once
#include <unordered_map>
#include "key.h"
#include "util/map.h"
#include "value.h"

class KVStore : public Object {
   public:
    std::unordered_map<Key, Value*> items_;
    // Map* items_;

    KVStore() : Object() {
        // items_ = new Map();
        items_ = std::unordered_map<Key, Value*>();
    }

    virtual ~KVStore() {
        // delete items_;
    }

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key* k) {
        // return items_->contains(k);
        return items_.find(*k) == items_.end();
    }

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key k) {
        // return items_->contains(k);
        return items_.find(k) != items_.end();
    }

    /**
     * Gets the value at the given key.
     * Returns a copy of the value.
     * @arg k  the key
     * @return the value
     */
    // virtual Value* get(Key* k) {
    //     return static_cast<Value*>(items_->get(k))->clone();
    // }

    /**
     * Gets the value at the given key.
     * Returns a copy of the value.
     * @arg k  the key
     * @return the value
     */
    virtual Value* get(Key k) {
        return items_[k];
    }

    // /**
    //  * Waits until there is a value at the given key and then gets it.
    //  * @arg k  the key
    //  * @return the value
    //  */
    // virtual Value* wait_and_get(Key* k) {
    //     while (!in(k)) {
    //     }
    //     return get(k);
    // }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    virtual Value* wait_and_get(Key k) {
        while (!in(k)) {
        }
        return get(k);
    }

    // /**
    //  * Puts the value at the given key.
    //  * Copies the Key and consumes the Value.
    //  * @arg k  the key to put the value at
    //  * @arg v  the value to put in the store
    //  */
    // virtual void put(Key* k, Value* v) {
    //     items_->add(k->clone(), v);
    // }

    /**
     * Puts the value at the given key.
     * Copies the Key and consumes the Value.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    virtual void put(Key k, Value* v) {
        if (items_.find(k) != items_.end()) {
            delete items_[k];
        }
        items_[k] = v;
    }
};
