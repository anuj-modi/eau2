#pragma once
#include "dataframe/dataframe.h"
#include "key.h"
#include "kvstore.h"
#include "util/serial.h"

class KDStore : public Object {
   public:
    KVStore* store_;

    KDStore(KVStore* kv) : Object() {
        store_ = kv;
    }

    virtual ~KDStore() {}

    /**
     * Checks if the given key is in the kvstore.
     * @arg k  the key
     * @return if it exists in the store
     */
    virtual bool in(Key& k) {
        return store_->in(k);
    }

    /**
     * Gets the value at the given key.
     * @arg k  the key
     * @return the value
     */
    DataFrame* get(Key& k) {
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        DataFrame* df = new DataFrame(&d, store_);
        delete v;
        return df;
    }

    /**
     * Gets underlying KVStore.
     * @return the kvstore
     */
    KVStore* get_kvstore() {
        return store_;
    }

    /**
     * Waits until there is a value at the given key and then gets it.
     * @arg k  the key
     * @return the value
     */
    DataFrame* waitAndGet(Key& k) {
        Value* v = store_->waitAndGet(k);
        Deserializer d(v->get_bytes(), v->size());
        DataFrame* df = new DataFrame(&d, store_);
        delete v;
        return df;
    }

    /**
     * Puts the value at the given key.
     * @arg k  the key to put the value at
     * @arg v  the value to put in the store
     */
    void put(Key& k, DataFrame* df) {
        Serializer s;
        df->serialize(&s);
        Value* v = new Value(s.get_bytes(), s.size());
        store_->put(k, v);
    }
};

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, double* vals) {
    DoubleColumn* dc = new DoubleColumn(kd->get_kvstore());
    for (size_t i = 0; i < size; i++) {
        dc->push_back(vals[i]);
    }
    DataFrame* df = new DataFrame(dc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, int* vals) {
    IntColumn* ic = new IntColumn(kd->get_kvstore());
    for (size_t i = 0; i < size; i++) {
        ic->push_back(vals[i]);
    }
    DataFrame* df = new DataFrame(ic, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, bool* vals) {
    BoolColumn* bc = new BoolColumn(kd->get_kvstore());
    for (size_t i = 0; i < size; i++) {
        bc->push_back(vals[i]);
    }
    DataFrame* df = new DataFrame(bc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromArray(Key* k, KDStore* kd, size_t size, String** vals) {
    StringColumn* sc = new StringColumn(kd->get_kvstore());
    for (size_t i = 0; i < size; i++) {
        sc->push_back(vals[i]);
    }
    DataFrame* df = new DataFrame(sc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, double val) {
    DoubleColumn* dc = new DoubleColumn(kd->get_kvstore());
    dc->push_back(val);
    DataFrame* df = new DataFrame(dc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, int val) {
    IntColumn* ic = new IntColumn(kd->get_kvstore());
    ic->push_back(val);
    DataFrame* df = new DataFrame(ic, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, bool val) {
    BoolColumn* bc = new BoolColumn(kd->get_kvstore());
    bc->push_back(val);
    DataFrame* df = new DataFrame(bc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, String* val) {
    StringColumn* sc = new StringColumn(kd->get_kvstore());
    sc->push_back(val);
    DataFrame* df = new DataFrame(sc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}
