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
    std::vector<Key> segments_;
    KVStore* store_;
    size_t size_;
    String* col_id_;

    Column(KVStore* store) : Object() {
        size_ = 0;
        segments_ = std::vector<Key>();
        store_ = store;
        StrBuff buff;
        char c[2];
        c[1] = '\0';
        for (size_t i = 0; i < 10; i++) {
            c[0] = ALPHA[rand() % ALPHA_SIZE];
            buff.c(c);
        }
        col_id_ = buff.get();
    }

    /**
     * Not providing this constructor with the same KVStore as
     * the serialized column is undefined behavior.
     */
    Column(KVStore* store, Deserializer* d) : Object() {
        store_ = store;
        size_ = d->get_size_t();
        col_id_ = d->get_string();
        size_t num_segments = d->get_size_t();
        segments_ = std::vector<Key>();
        for (size_t i = 0; i < num_segments; i++) {
            segments_.push_back(Key(d));
        }
    }

    virtual ~Column() {
        delete col_id_;
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
     * Marks the column as final and doesn't allow for any more additions.
     */
    void finalize() {

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
        s->add_string(col_id_);
        s->add_size_t(segments_.size());
        for (size_t i = 0; i < segments_.size(); i++) {
            segments_[i].serialize(s);
        }
    }

    virtual void expand_() {
        char buffer[15];
        snprintf(buffer, 15, "%s_%zu", col_id_->c_str(), segments_.size());
        Key new_k = Key(buffer);
        segments_.push_back(new_k);
        Serializer s;
        s.add_size_t(0);
        Value* v = new Value(s.get_bytes(), s.size());
        store_->put(new_k, v);
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
        if (size_ == segments_.size() * SEGMENT_CAPACITY) {
            expand_();
        }
        Key& k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        IntArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        Value* new_v = new Value(s.get_bytes(), s.size());
        store_->put(k, new_v);
        delete v;
        size_ += 1;
    }

    int get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key& k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        IntArray temp(&d);
        delete v;
        return temp.get(index_in_seg);
    }

    IntColumn* as_int() {
        return this;
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
        if (size_ == segments_.size() * SEGMENT_CAPACITY) {
            expand_();
        }
        Key& k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        BoolArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete v;
        size_ += 1;
    }

    bool get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key& k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        BoolArray temp(&d);
        delete v;
        return temp.get(index_in_seg);
    }

    BoolColumn* as_bool() {
        return this;
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
        if (size_ == segments_.size() * SEGMENT_CAPACITY) {
            expand_();
        }
        Key& k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        DoubleArray temp(&d);
        temp.push_back(val);
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        delete v;
        size_ += 1;
    }

    double get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key& k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        DoubleArray temp(&d);
        delete v;
        return temp.get(index_in_seg);
    }

    DoubleColumn* as_double() {
        return this;
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
        if (size_ == segments_.size() * SEGMENT_CAPACITY) {
            expand_();
        }
        Key& k = segments_.back();
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        StringArray temp(&d);
        temp.push_back(val->clone());
        Serializer s;
        temp.serialize(&s);
        store_->put(k, new Value(s.get_bytes(), s.size()));
        temp.delete_items();
        delete v;
        size_ += 1;
    }

    String* get(size_t idx) {
        assert(idx < size());
        size_t segment_index = idx / SEGMENT_CAPACITY;
        int index_in_seg = idx % SEGMENT_CAPACITY;
        Key& k = segments_[segment_index];
        Value* v = store_->get(k);
        Deserializer d(v->get_bytes(), v->size());
        StringArray temp(&d);
        delete v;
        String* result = temp.get(index_in_seg)->clone();
        temp.delete_items();
        return result;
    }

    StringColumn* as_string() {
        return this;
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
