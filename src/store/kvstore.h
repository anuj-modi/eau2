#pragma once
#include <map>
#include "key.h"
#include "value.h"

class KVStore : public Object {
   public:
    std::map<Key*, Value*> items_;

    KVStore() : Object() {
        items_ = std::map<Key*, Value*>();
    }

    ~KVStore() {
        // TODO should we own and delete
    }

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    bool in(Key* k) {
        std::map<Key*, Value*>::iterator check;
        check = items_.find(k);
        return check != items_.end();
    }

    /**
     * Gets the value at the given key.
     * @arg k  the key
     * @return the value
     */
    Value* get(Key* k) {
        assert(in(k));
        return items_[k];
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    Value* wait_and_get(Key* k) {
        while (!in(k)) {
        }
        return items_[k];
    }

    /**
     * Puts the value at the given key.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    void put(Key* k, Value* v) {
        items_[k] = v;
    }
};
