#pragma once
#include <assert.h>
#include <stdarg.h>

#include <array>
#include <random>
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

static const size_t DEFAULT_SEGMENT_CAPACITY = 5242880 * 2;
static const char* ALPHA = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const size_t ALPHA_SIZE = strlen(ALPHA);

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type.
 * Author: gomes.chri, modi.an
 */
class Column : public Object {
   public:
    std::vector<Key> segments_;
    KVStore* store_;
    bool finalized_;
    size_t size_;
    String* col_id_;
    Array* cache_;
    Key cache_key_;
    size_t curr_node_;
    const size_t segment_capacity_;

    Column(KVStore* store, size_t segment_capacity)
        : Object(), segment_capacity_(segment_capacity) {
        size_ = 0;
        segments_ = std::vector<Key>();
        store_ = store;
        finalized_ = false;
        StrBuff buff;
        char c[2];
        c[1] = '\0';
        // Brought in from cppreference.com
        std::random_device rd;   // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> dis(0, ALPHA_SIZE);

        for (size_t i = 0; i < 10; i++) {
            c[0] = ALPHA[dis(gen)];
            buff.c(c);
        }
        col_id_ = buff.get();
        curr_node_ = 0;
        expand_();
    }

    Column(KVStore* store) : Column(store, DEFAULT_SEGMENT_CAPACITY) {}

    /**
     * Not providing this constructor with the same KVStore as
     * the serialized column is undefined behavior.
     */
    Column(KVStore* store, Deserializer* d)
        : Object(), segment_capacity_(DEFAULT_SEGMENT_CAPACITY) {
        store_ = store;
        finalized_ = true;
        size_ = d->get_size_t();
        col_id_ = d->get_string();
        size_t num_segments = d->get_size_t();
        segments_ = std::vector<Key>();
        curr_node_ = 0;
        for (size_t i = 0; i < num_segments; i++) {
            segments_.push_back(Key(d));
        }
    }

    virtual ~Column() {
        delete col_id_;
        delete cache_;
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
     * Gets list of indices on local node.
     * @arg node  index of local node
     * @return list of indices
     */
    std::vector<size_t> local_indices() {
        assert(finalized_);
        std::vector<size_t> indices = std::vector<size_t>();
        for (size_t i = 0; i < segments_.size(); i++) {
            if (segments_[i].get_node() == store_->this_node()) {
                size_t start = i * segment_capacity_;
                size_t end;
                if (i < segments_.size() - 1 || size_ % segment_capacity_ == 0) {
                    end = start + segment_capacity_;
                } else {
                    end = start + (size_ % segment_capacity_);
                }
                for (size_t j = start; j < end; j++) {
                    indices.push_back(j);
                }
            }
        }
        return indices;
    }

    /**
     * Marks the column as final and doesn't allow for any more additions.
     * Column cannot already be finalized.
     */
    void finalize() {
        if (finalized_) {
            return;
        }
        finalized_ = true;
        put_in_store_();
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
     * Column must be finalized.
     */
    virtual void serialize(Serializer* s) {
        assert(finalized_);
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
        Key new_k(buffer, curr_node_);
        segments_.push_back(new_k);
        curr_node_ = (curr_node_ + 1) % store_->num_nodes();
    }

    virtual void put_in_store_() {
        Serializer s;
        cache_->serialize(&s);
        Value* v = new Value(s.get_bytes(), s.size());
        store_->put(segments_.back(), v);
    }
};

/*************************************************************************
 * IntColumn::
 * Holds int values.
 * Author: gomes.chri, modi.an
 */
class IntColumn : public Column {
   public:
    IntColumn(KVStore* store, size_t segment_capacity) : Column(store, segment_capacity) {
        cache_ = new IntArray(segment_capacity_);
    }

    IntColumn(KVStore* store) : IntColumn(store, DEFAULT_SEGMENT_CAPACITY) {}

    IntColumn(KVStore* store, Deserializer* d) : Column(store, d) {
        cache_ = new IntArray(segment_capacity_);
    }

    virtual ~IntColumn() {}

    void expand_() {
        Column::expand_();
        delete cache_;
        cache_ = new IntArray(segment_capacity_);
    }

    /**
     * Pushes item onto the column.
     * Column must not be finalized.
     * @arg val  the value to add
     */
    void push_back(int val) {
        assert(!finalized_);
        if (size_ == segments_.size() * segment_capacity_) {
            put_in_store_();
            expand_();
        }
        static_cast<IntArray*>(cache_)->push_back(val);
        size_ += 1;
    }

    /**
     * Gets the item at the given index.
     * Column must be finalized.
     * @arg idx  the index to get at
     * @return the item at the index
     */
    int get(size_t idx) {
        assert(idx < size());
        assert(finalized_);
        size_t segment_index = idx / segment_capacity_;
        size_t index_in_seg = idx % segment_capacity_;
        Key& k = segments_[segment_index];
        if (!k.equals(&cache_key_)) {
            delete cache_;
            Value* v = store_->get(k);
            Deserializer d(v->get_bytes(), v->size());
            cache_ = new IntArray(&d);
            cache_key_ = k;
            delete v;
        }
        return cache_->get_int(index_in_seg);
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
 * Author: gomes.chri, modi.an
 */
class BoolColumn : public Column {
   public:
    BoolColumn(KVStore* store, size_t segment_capacity) : Column(store, segment_capacity) {
        cache_ = new BoolArray(segment_capacity_);
    }

    BoolColumn(KVStore* store) : BoolColumn(store, DEFAULT_SEGMENT_CAPACITY) {}

    BoolColumn(KVStore* store, Deserializer* d) : Column(store, d) {
        cache_ = new BoolArray(segment_capacity_);
    }

    virtual ~BoolColumn() {}

    void expand_() {
        Column::expand_();
        delete cache_;
        cache_ = new BoolArray(segment_capacity_);
    }

    /**
     * Pushes item onto the column.
     * Column must not be finalized.
     * @arg val  the value to add
     */
    void push_back(bool val) {
        assert(!finalized_);
        if (size_ == segments_.size() * segment_capacity_) {
            put_in_store_();
            expand_();
        }
        static_cast<BoolArray*>(cache_)->push_back(val);
        size_ += 1;
    }

    /**
     * Gets the item at the given index.
     * Column must be finalized.
     * @arg idx  the index to get at
     * @return the item at the index
     */
    bool get(size_t idx) {
        assert(idx < size());
        assert(finalized_);
        size_t segment_index = idx / segment_capacity_;
        int index_in_seg = idx % segment_capacity_;
        Key& k = segments_[segment_index];
        if (!k.equals(&cache_key_)) {
            delete cache_;
            Value* v = store_->get(k);
            Deserializer d(v->get_bytes(), v->size());
            cache_ = new BoolArray(&d);
            cache_key_ = k;
            delete v;
        }
        return cache_->get_bool(index_in_seg);
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
 * Author: gomes.chri, modi.an
 */
class DoubleColumn : public Column {
   public:
    DoubleColumn(KVStore* store, size_t segment_capacity) : Column(store, segment_capacity) {
        cache_ = new DoubleArray(segment_capacity_);
    }

    DoubleColumn(KVStore* store) : DoubleColumn(store, DEFAULT_SEGMENT_CAPACITY) {}

    DoubleColumn(KVStore* store, Deserializer* d) : Column(store, d) {
        cache_ = new DoubleArray(segment_capacity_);
    }

    virtual ~DoubleColumn() {}

    void expand_() {
        Column::expand_();
        delete cache_;
        cache_ = new DoubleArray(segment_capacity_);
    }

    /**
     * Pushes item onto the column.
     * Column must not be finalized.
     * @arg val  the value to add
     */
    void push_back(double val) {
        assert(!finalized_);
        if (size_ == segments_.size() * segment_capacity_) {
            put_in_store_();
            expand_();
        }
        static_cast<DoubleArray*>(cache_)->push_back(val);
        size_ += 1;
    }

    /**
     * Gets the item at the given index.
     * Column must be finalized.
     * @arg idx  the index to get at
     * @return the item at the index
     */
    double get(size_t idx) {
        assert(idx < size());
        assert(finalized_);
        size_t segment_index = idx / segment_capacity_;
        int index_in_seg = idx % segment_capacity_;
        Key& k = segments_[segment_index];
        if (!k.equals(&cache_key_)) {
            delete cache_;
            Value* v = store_->get(k);
            Deserializer d(v->get_bytes(), v->size());
            cache_ = new DoubleArray(&d);
            cache_key_ = k;
            delete v;
        }
        return cache_->get_double(index_in_seg);
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
 * Holds string pointers. Strings are copied when added and nullptr is a valid
 * value.
 * Author: gomes.chri, modi.an
 */
class StringColumn : public Column {
   public:
    StringColumn(KVStore* store, size_t segment_capacity) : Column(store, segment_capacity) {
        cache_ = new StringArray(segment_capacity_);
    }

    StringColumn(KVStore* store) : StringColumn(store, DEFAULT_SEGMENT_CAPACITY) {}

    StringColumn(KVStore* store, Deserializer* d) : Column(store, d) {
        cache_ = new StringArray(segment_capacity_);
    }

    virtual ~StringColumn() {}

    void expand_() {
        Column::expand_();
        delete cache_;
        cache_ = new StringArray(segment_capacity_);
    }

    /**
     * Pushes item onto the column.
     * Column must not be finalized.
     * @arg val  the value to add
     */
    void push_back(String* val) {
        assert(!finalized_);
        if (size_ == segments_.size() * segment_capacity_) {
            put_in_store_();
            expand_();
        }
        static_cast<StringArray*>(cache_)->push_back(val->clone());
        size_ += 1;
    }

    /**
     * Gets the item at the given index.
     * Column must be finalized.
     * @arg idx  the index to get at
     * @return the item at the index
     */
    String* get(size_t idx) {
        assert(idx < size());
        assert(finalized_);
        size_t segment_index = idx / segment_capacity_;
        int index_in_seg = idx % segment_capacity_;
        Key& k = segments_[segment_index];
        if (!k.equals(&cache_key_)) {
            delete cache_;
            Value* v = store_->get(k);
            Deserializer d(v->get_bytes(), v->size());
            cache_ = new StringArray(&d);
            cache_key_ = k;
            delete v;
        }
        return cache_->get_string(index_in_seg)->clone();
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
