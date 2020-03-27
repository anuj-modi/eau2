#pragma once
#include <assert.h>
#include <stdarg.h>
#include <array>
#include <vector>
#include "store/kvstore.h"
#include "util/array.h"
#include "util/serial.h"

/**
 * Enum for the different types of SoR columns this code supports.
 */
enum class ColumnType { STRING, INTEGER, DOUBLE, BOOL, UNKNOWN };

class KDStore;
class IntColumn;
class DoubleColumn;
class BoolColumn;
class StringColumn;

static const size_t SEGMENT_CAPACITY = 128;
static const char* ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const size_t ALPHA_SIZE = 52;

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */
class Column : public Object {
   public:
    std::vector<Key*> segments_;
    KVStore* store_;
    size_t size_;
    size_t curr_segment_size_;
    String* col_id;

    Column(KVStore* store) : Object() {
        size_ = 0;
        curr_segment_size_ = 0;
        segments_ = std::vector<Key*>();
        store_ = store;
        StrBuff buff;
        for (size_t i = 0; i < 10; i++) {
            buff.c(ALPHA[rand() % ALPHA_SIZE]);
        }
        col_id = buff.get();
        expand_();
        // printf("%s", col_id->c_str());
    }

    /**
     * Not providing this constructor with the same KVStore as
     * the serialized column is undefined behavior.
     */
    Column(KVStore* store, Deserializer* d) : Object() {
        store_ = store;
        size_ = d->get_size_t();
        curr_segment_size_ = d->get_size_t();
        col_id = d->get_string();
        size_t num_segments = d->get_size_t();
        segments_ = std::vector<Key*>();
        for (size_t i = 0; i < num_segments; i++) {
            segments_.push_back(new Key(d));
        }
    }

    virtual ~Column() {
        delete col_id;
    }

    /** Type converters: Return same column under its actual type, or
     *  nullptr if of the wrong type.  */
    virtual IntColumn* as_int() {
        return nullptr;
    }
    virtual BoolColumn* as_bool() {
        return nullptr;
    }
    virtual DoubleColumn* as_double() {
        return nullptr;
    }
    virtual StringColumn* as_string() {
        return nullptr;
    }

    /** Type appropriate push_back methods. Calling the wrong method is
     * undefined behavior. **/
    virtual void push_back(int val) {
        assert(false);
    }

    virtual void push_back(bool val) {
        assert(false);
    }

    virtual void push_back(double val) {
        assert(false);
    }

    virtual void push_back(String* val) {
        assert(false);
    }

    /** Returns the number of elements in the column. */
    virtual size_t size() {
        return size_;
    }

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'D'. */
    virtual char get_type() {
        assert(false);
        return 'U';
    }

    /**
     * Makes a copy of the column.
     */
    virtual Column* clone() {
        assert(false);
        return nullptr;
    }

    /**
     * Serializes the column.
     */
    virtual void serialize(Serializer* s) {
        s->add_size_t(size_);
        s->add_size_t(curr_segment_size_);
        s->add_string(col_id);
        s->add_size_t(segments_.size());
        for (size_t i = 0; i < size_; i++) {
            segments_[i]->serialize(s);
        }
    }

    virtual void expand_() {
        char buffer[15];
        snprintf(buffer, 15, "%s%zu", col_id->c_str(), segments_.size() + 1);
        Key* new_k = new Key(buffer);
        segments_.push_back(new_k);
        Serializer s;
        s.add_size_t(0);
        store_->put(new_k, new Value(s.get_bytes(), s.size()));
        curr_segment_size_ = 0;
    }
};

/*************************************************************************
 * IntColumn::
 * Holds int values.
 */
class IntColumn : public Column {
   public:
    IntColumn(KVStore* store) : Column(store) {}

    IntColumn(KVStore* store, Deserializer* d) : Column(store, d) {}

    virtual ~IntColumn() {}

    void push_back(int val) {
        if (curr_segment_size_ == SEGMENT_CAPACITY) {
            expand_();
        }
        Key* k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        IntArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete v;
        delete k;
        size_ += 1;
        curr_segment_size_ += 1;
    }

    int get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        IntArray temp(&d);
        delete k;
        delete v;
        return temp.get(index_in_seg);
        // TODO may have memory issues
    }

    IntColumn* as_int() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, int val) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        IntArray temp(&d);
        temp.set(index_in_seg, val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete k;
        delete v;
    }

    virtual char get_type() {
        return 'I';
    }

    virtual Column* clone() {
        IntColumn* result = new IntColumn(store_);
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }
};

/*************************************************************************
 * BoolColumn::
 * Holds bool values.
 */
class BoolColumn : public Column {
   public:
    BoolColumn(KVStore* store) : Column(store) {}

    BoolColumn(KVStore* store, Deserializer* d) : Column(store, d) {}

    virtual ~BoolColumn() {}

    void push_back(bool val) {
        if (curr_segment_size_ == SEGMENT_CAPACITY) {
            expand_();
        }
        Key* k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        BoolArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete v;
        delete k;
        size_ += 1;
        curr_segment_size_ += 1;
    }

    bool get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        BoolArray temp(&d);
        delete k;
        delete v;
        return temp.get(index_in_seg);
        // TODO may have memory issues
    }

    BoolColumn* as_bool() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, bool val) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        BoolArray temp(&d);
        temp.set(index_in_seg, val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete k;
        delete v;
    }

    virtual char get_type() {
        return 'B';
    }

    virtual Column* clone() {
        BoolColumn* result = new BoolColumn(store_);
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }
};

/*************************************************************************
 * DoubleColumn::
 * Holds double values.
 */
class DoubleColumn : public Column {
   public:
    DoubleColumn(KVStore* store) : Column(store) {}

    DoubleColumn(KVStore* store, Deserializer* d) : Column(store, d) {}

    virtual ~DoubleColumn() {}

    void push_back(double val) {
        if (curr_segment_size_ == SEGMENT_CAPACITY) {
            expand_();
        }
        Key* k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        DoubleArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete v;
        delete k;
        size_ += 1;
        curr_segment_size_ += 1;
    }

    double get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        DoubleArray temp(&d);
        delete k;
        delete v;
        return temp.get(index_in_seg);
        // TODO may have memory issues
    }

    DoubleColumn* as_double() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, double val) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        DoubleArray temp(&d);
        temp.set(index_in_seg, val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete k;
        delete v;
    }

    virtual size_t size() {
        return size_;
    }

    virtual char get_type() {
        return 'D';
    }

    virtual Column* clone() {
        DoubleColumn* result = new DoubleColumn(store_);
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }
};

// Other primitive column classes similar...

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column {
   public:
    StringColumn(KVStore* store) : Column(store) {}

    StringColumn(KVStore* store, Deserializer* d) : Column(store, d) {}

    virtual ~StringColumn() {}

    void push_back(String* val) {
        if (curr_segment_size_ == SEGMENT_CAPACITY) {
            expand_();
        }
        Key* k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        StringArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete v;
        delete k;
        size_ += 1;
        curr_segment_size_ += 1;
    }

    String* get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        StringArray temp(&d);
        delete k;
        delete v;
        return temp.get(index_in_seg);
        // TODO may have memory issues
    }

    StringColumn* as_string() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, String* val) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key* k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        StringArray temp(&d);
        temp.set(index_in_seg, val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete k;
        delete v;
    }

    virtual char get_type() {
        return 'S';
    }

    virtual Column* clone() {
        StringColumn* result = new StringColumn(store_);
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }
};
