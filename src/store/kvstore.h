#pragma once
#include "key.h"
#include "util/map.h"
#include "value.h"

class KVStore : public Object {
   public:
    Map* items_;

    KVStore() : Object() {
        items_ = new Map();
    }

    virtual ~KVStore() {
        delete items_;
    }

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key* k) {
        return items_->contains(k);
    }

    /**
     * Gets the value at the given key.
     * @arg k  the key
     * @return the value
     */
    virtual Value* get(Key* k) {
        return static_cast<Value*>(items_->get(k))->clone();
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    virtual Value* wait_and_get(Key* k) {
        while (!in(k)) {
        }
        return get(k);
    }

    /**
     * Puts the value at the given key.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    virtual void put(Key* k, Value* v) {
        items_->add(k, v);
    }
};
