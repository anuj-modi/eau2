#pragma once
#include "dataframe/dataframe.h"
#include "kvstore.h"

class KDStore : public Object {
   public:
    KVStore* store_;

    KDStore(KVStore* kv) : Object() {
        store_ = kv;
    }

    ~KDStore() {}

    /**
     * Gets the value at the given key.
     * @arg k  the key
     * @return the value
     */
    DataFrame* get(Key* k) {
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->get_size());
        DataFrame* df = new DataFrame(&d);
        return df;
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    Value* wait_and_get(Key* k) {
        Value* v = store_->wait_and_gets(k);
        Deserializer d(v->get_bytes(), v->get_size());
        DataFrame* df = new DataFrame(&d);
        return df;
    }

    /**
     * Puts the value at the given key.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    void put(Key* k, Value* v) {
        store_[k] = 
    }
};