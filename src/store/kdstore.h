#pragma once
#include "dataframe/dataframe.h"
#include "kvstore.h"

// TODO test kdstore methods
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
    DataFrame* wait_and_get(Key* k) {
        Value* v = store_->wait_and_get(k);
        Deserializer d(v->get_bytes(), v->get_size());
        DataFrame* df = new DataFrame(&d);
        return df;
    }

    /**
     * Puts the value at the given key.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    void put(Key* k, DataFrame* df) {
        Serializer s;
        df->serialize(&s);
        Value* v = new Value(s.get_bytes(), s.size());
        store_->put(k, v);
    }
};