#pragma once
#include "dataframe/dataframe.h"
#include "key.h"
#include "kvstore.h"
#include "util/serial.h"

class KDStore : public Object {
   public:
    KVStore* store_;

    KDStore() : Object() {
        store_ = new KVStore();
    }

    KDStore(KVStore* kv) : Object() {
        store_ = kv;
    }

    ~KDStore() {}

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key* k) {
        return store_->in(k);
    }

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

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, double* vals) {
    Schema s("D");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    for (size_t i = 0; i < size; i++) {
        r.set(0, vals[i]);
        df->add_row(r);
    }
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, int* vals) {
    Schema s("I");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    for (size_t i = 0; i < size; i++) {
        r.set(0, vals[i]);
        df->add_row(r);
    }
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, bool* vals) {
    Schema s("B");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    for (size_t i = 0; i < size; i++) r.set(0, vals[i]);
    df->add_row(r);
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, String** vals) {
    Schema s("S");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    for (size_t i = 0; i < size; i++) {
        r.set(0, vals[i]);
        df->add_row(r);
    }
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, double val) {
    Schema s("D");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    r.set(0, val);
    df->add_row(r);
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, int val) {
    Schema s("I");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    r.set(0, val);
    df->add_row(r);
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, bool val) {
    Schema s("B");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    r.set(0, val);
    df->add_row(r);
    kd->put(k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, String* val) {
    Schema s("S");
    Row r(s);
    DataFrame* df = new DataFrame(s);
    r.set(0, val);
    df->add_row(r);
    kd->put(k, df);
    return df;
}
