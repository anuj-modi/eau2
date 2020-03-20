#pragma once
#include <map>
#include "key.h"
#include "value.h"

// TODO test kvstore methods
class KVStore : public Object {
   public:
    std::map<Key*, Value*> items_;

    KVStore() : Object() {
        items_ = std::map<Key*, Value*>();
    }

    ~KVStore() {
        std::map<Key*, Value*>::iterator it;
        for (it = items_.begin(); it != items_.end(); it++) {
            delete it->first;
            delete it->second;
        }
    }

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key* k) {
        std::map<Key*, Value*>::iterator check;
        check = items_.find(k);
        return check != items_.end();
    }

    /**
     * Gets the value at the given key.
     * @arg k  the key
     * @return the value
     */
    virtual Value* get(Key* k) {
        assert(in(k));
        return items_[k];
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
        items_[k] = v;
    }
};
