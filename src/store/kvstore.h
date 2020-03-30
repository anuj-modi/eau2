#pragma once
#include <unordered_map>
#include "key.h"
#include "value.h"

/**
 * Key value store.
 * Author: gomes.chri, modi.an
 */
class KVStore : public Object {
   public:
    std::unordered_map<Key, Value*> items_;

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
        return items_.find(k) != items_.end();
    }

    /**
     * Gets the value at the given key.
     * Returns a copy of the value.
     * @arg k  the key
     * @return the value
     */
    virtual Value* get(Key& k) {
        return items_[k]->clone();
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    virtual Value* waitAndGet(Key& k) {
        while (!in(k)) {
        }
        return get(k);
    }

    /**
     * Puts the value at the given key.
     * Copies the Key and consumes the Value.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    virtual void put(Key& k, Value* v) {
        if (items_.find(k) != items_.end()) {
            delete items_[k];
            items_[k] = v;
        } else {
            items_[Key(k)] = v;
        }
    }
};
