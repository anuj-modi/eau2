#pragma once
#include <assert.h>
#include <stdarg.h>
#include <array>
#include <vector>
#include "store/kvstore.h"
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
        Serializer s;
        Value* v = store_->get(segments_.back());
        if (v->size() > 0) {
            s.add_buffer(v->get_bytes(), v->size());
        }
        s.add_int(val);
        store_->put(segments_.back(), new Value(s.get_bytes(), s.size()));
        delete v;
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
        IntArray* temp = new IntArray();
        for (size_t i = 0; 0 < )
    }

    IntColumn* as_int() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, int val) {
        assert(idx < size());
        segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY] = val;
    }

    virtual char get_type() {
        return 'I';
    }

    virtual Column* clone() {
        IntColumn* result = new IntColumn();
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }

    virtual void serialize(Serializer* s) {
        s->add_size_t(size_);
        for (size_t i = 0; i < size_; i++) {
            s->add_int(get(i));
        }
    }
};

// /*************************************************************************
//  * BoolColumn::
//  * Holds bool values.
//  */
// class BoolColumn : public Column {
//    public:
//     size_t size_;
//     size_t capacity_;
//     size_t num_segments_;
//     size_t segments_capacity_;
//     bool** segments_;

//     BoolColumn() : Column() {
//         size_ = 0;
//         capacity_ = SEGMENT_CAPACITY;
//         num_segments_ = 1;
//         segments_capacity_ = 10;
//         segments_ = new bool*[segments_capacity_];
//         segments_[0] = new bool[SEGMENT_CAPACITY];
//     }

//     BoolColumn(Deserializer* d) : BoolColumn() {
//         size_t num_items = d->get_size_t();
//         for (size_t i = 0; i < num_items; i++) {
//             push_back(d->get_bool());
//         }
//     }

//     virtual ~BoolColumn() {
//         for (size_t i = 0; i < num_segments_; i++) {
//             delete[] segments_[i];
//         }
//         delete[] segments_;
//     }

//     void push_back(bool val) {
//         if (curr_segment_size_ == SEGMENT_CAPACITY) {
//             expand_();
//         }
//         Serializer s;
//         Value* v = store_->get(segments_.back());
//         if (v->size() > 0) {
//             s.add_buffer(v->get_bytes(), v->size());
//         }
//         s.add_bool(val);
//         store_->put(segments_.back(), new Value(s.get_bytes(), s.size()));
//         delete v;
//     }

//     bool get(size_t idx) {
//         assert(idx < size());
//         return segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY];
//     }

//     BoolColumn* as_bool() {
//         return this;
//     }

//     /** Set value at idx. An out of bound idx is undefined.  */
//     void set(size_t idx, bool val) {
//         assert(idx < size());
//         segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY] = val;
//     }

//     virtual char get_type() {
//         return 'B';
//     }

//     virtual Column* clone() {
//         BoolColumn* result = new BoolColumn();
//         for (size_t i = 0; i < size(); i++) {
//             result->push_back(get(i));
//         }
//         return result;
//     }
// };

// /*************************************************************************
//  * DoubleColumn::
//  * Holds double values.
//  */
// class DoubleColumn : public Column {
//    public:
//     size_t size_;
//     size_t capacity_;
//     size_t num_segments_;
//     size_t segments_capacity_;
//     double** segments_;

//     DoubleColumn() : Column() {
//         size_ = 0;
//         capacity_ = SEGMENT_CAPACITY;
//         num_segments_ = 1;
//         segments_capacity_ = 10;
//         segments_ = new double*[segments_capacity_];
//         segments_[0] = new double[SEGMENT_CAPACITY];
//     }

//     DoubleColumn(Deserializer* d) : DoubleColumn() {
//         size_t num_items = d->get_size_t();
//         for (size_t i = 0; i < num_items; i++) {
//             push_back(d->get_double());
//         }
//     }

//     virtual ~DoubleColumn() {
//         for (size_t i = 0; i < num_segments_; i++) {
//             delete[] segments_[i];
//         }
//         delete[] segments_;
//     }

//     void push_back(double val) {
//         if (curr_segment_size_ == SEGMENT_CAPACITY) {
//             expand_();
//         }
//         Serializer s;
//         Value* v = store_->get(segments_.back());
//         if (v->size() > 0) {
//             s.add_buffer(v->get_bytes(), v->size());
//         }
//         s.add_bool(val);
//         store_->put(segments_.back(), new Value(s.get_bytes(), s.size()));
//         delete v;
//     }

//     double get(size_t idx) {
//         assert(idx < size());
//         return segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY];
//     }

//     DoubleColumn* as_double() {
//         return this;
//     }

//     /** Set value at idx. An out of bound idx is undefined.  */
//     void set(size_t idx, double val) {
//         assert(idx < size());
//         segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY] = val;
//     }

//     virtual size_t size() {
//         return size_;
//     }

//     virtual char get_type() {
//         return 'D';
//     }

//     virtual Column* clone() {
//         DoubleColumn* result = new DoubleColumn();
//         for (size_t i = 0; i < size(); i++) {
//             result->push_back(get(i));
//         }
//         return result;
//     }

//     virtual void serialize(Serializer* s) {
//         s->add_size_t(size_);
//         for (size_t i = 0; i < size_; i++) {
//             s->add_double(get(i));
//         }
//     }
// };

// // Other primitive column classes similar...

// /*************************************************************************
//  * StringColumn::
//  * Holds string pointers. The strings are external.  Nullptr is a valid
//  * value.
//  */
// class StringColumn : public Column {
//    public:
//     size_t size_;
//     size_t capacity_;
//     size_t num_segments_;
//     size_t segments_capacity_;
//     String*** segments_;

//     StringColumn() : Column() {
//         size_ = 0;
//         capacity_ = SEGMENT_CAPACITY;
//         num_segments_ = 1;
//         segments_capacity_ = 10;
//         segments_ = new String**[segments_capacity_];
//         segments_[0] = new String*[SEGMENT_CAPACITY];
//     }

//     // StringColumn(int n, ...) : StringColumn() {
//     //     va_list args;
//     //     va_start(args, n);
//     //     String* val = nullptr;
//     //     for (int i = 0; i < n; i++) {
//     //         val = va_arg(args, String*);
//     //         push_back(val);
//     //     }
//     //     va_end(args);
//     // }

//     StringColumn(Deserializer* d) : StringColumn() {
//         size_t num_items = d->get_size_t();
//         for (size_t i = 0; i < num_items; i++) {
//             push_back(d->get_string());
//         }
//     }

//     virtual ~StringColumn() {
//         for (size_t i = 0; i < num_segments_; i++) {
//             delete[] segments_[i];
//         }
//         delete[] segments_;
//     }

//     void expand_() {
//         if (num_segments_ == segments_capacity_) {
//             size_t new_capacity = num_segments_ * 2;
//             String*** new_segments = new String**[new_capacity];
//             for (size_t i = 0; i < num_segments_; i++) {
//                 new_segments[i] = segments_[i];
//             }

//             delete[] segments_;
//             segments_ = new_segments;
//             segments_capacity_ = new_capacity;
//         }

//         segments_[num_segments_] = new String*[SEGMENT_CAPACITY];
//         capacity_ += SEGMENT_CAPACITY;
//         num_segments_ += 1;
//     }

//     void push_back(String* val) {
//         if (size() == capacity_) {
//             expand_();
//         }
//         segments_[num_segments_ - 1][size_ % SEGMENT_CAPACITY] = val;
//         size_ += 1;
//     }

//     String* get(size_t idx) {
//         assert(idx < size());
//         return segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY];
//     }

//     StringColumn* as_string() {
//         return this;
//     }

//     /** Set value at idx. An out of bound idx is undefined.  */
//     void set(size_t idx, String* val) {
//         assert(idx < size());
//         segments_[idx / SEGMENT_CAPACITY][idx % SEGMENT_CAPACITY] = val;
//     }

//     virtual char get_type() {
//         return 'S';
//     }

//     virtual Column* clone() {
//         StringColumn* result = new StringColumn();
//         for (size_t i = 0; i < size(); i++) {
//             result->push_back(get(i));
//         }
//         return result;
//     }
// };
