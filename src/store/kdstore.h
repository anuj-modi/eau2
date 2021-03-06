#pragma once
#include "dataframe/dataframe.h"
#include "key.h"
#include "kvstore.h"
#include "sorer/parser.h"
#include "util/serial.h"

/**
 * Wrapper to hold DataFrames in a KVStore.
 * Author: gomes.chri, modi.an
 */
class KDStore : public Object {
   public:
    KVStore* store_;

    KDStore(KVStore* kv) : Object() {
        store_ = kv;
    }

    virtual ~KDStore() {}

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
     * Puts the data frame at the given key.
     * @arg k  the key to put the value at
     * @arg df  the data frame
     */
    void put(Key& k, DataFrame* df) {
        Serializer s;
        df->serialize(&s);
        Value* v = new Value(s.get_bytes(), s.size());
        store_->put(k, v);
    }
};

// The following methods are only here because of really silly circular dependency issues.

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
    DoubleColumn* dc = new DoubleColumn(kd->get_kvstore(), 1);
    dc->push_back(val);
    DataFrame* df = new DataFrame(dc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, int val) {
    IntColumn* ic = new IntColumn(kd->get_kvstore(), 1);
    ic->push_back(val);
    DataFrame* df = new DataFrame(ic, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, bool val) {
    BoolColumn* bc = new BoolColumn(kd->get_kvstore(), 1);
    bc->push_back(val);
    DataFrame* df = new DataFrame(bc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromScalar(Key* k, KDStore* kd, String* val) {
    StringColumn* sc = new StringColumn(kd->get_kvstore(), 1);
    sc->push_back(val);
    DataFrame* df = new DataFrame(sc, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}

inline DataFrame* DataFrame::fromSorFile(Key* k, KDStore* kd, const char* file_name) {
    FILE* file = fopen(file_name, "r");
    SorParser parser(file, kd->get_kvstore());
    parser.guessSchema();
    parser.parseFile();
    ColumnSet* cols = parser.getColumnSet();
    DataFrame* df = new DataFrame(cols->getColumns(), kd->get_kvstore());
    kd->put(*k, df);
    fclose(file);
    return df;
}

inline DataFrame* DataFrame::fromVisitor(Key* k, KDStore* kd, const char* types, Writer& v) {
    Schema s(types);
    std::vector<Column*> cols = std::vector<Column*>();
    for (size_t i = 0; i < s.width(); i++) {
        switch (s.col_type(i)) {
            case 'S':
                cols.push_back(new StringColumn(kd->get_kvstore()));
                break;
            case 'I':
                cols.push_back(new IntColumn(kd->get_kvstore()));
                break;
            case 'B':
                cols.push_back(new BoolColumn(kd->get_kvstore()));
                break;
            case 'D':
                cols.push_back(new DoubleColumn(kd->get_kvstore()));
                break;
            default:
                assert(false);
        }
    }
    Row r(s);
    while (!v.done()) {
        v.visit(r);
        r.add_to_columns(cols);
    }
    DataFrame* df = new DataFrame(cols, kd->get_kvstore());
    kd->put(*k, df);
    return df;
}
