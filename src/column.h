#pragma once
#include <assert.h>
#include <stdarg.h>
#include "object.h"
#include "string.h"

class IntColumn;
class FloatColumn;
class BoolColumn;
class StringColumn;

static const int SEGMENT_SIZE = 128;

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */
class Column : public Object {
   public:
    Column() : Object() {}
    virtual ~Column() {}

    /** Type converters: Return same column under its actual type, or
     *  nullptr if of the wrong type.  */
    virtual IntColumn* as_int() {
        return nullptr;
    }
    virtual BoolColumn* as_bool() {
        return nullptr;
    }
    virtual FloatColumn* as_float() {
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
    virtual void push_back(float val) {
        assert(false);
    }
    virtual void push_back(String* val) {
        assert(false);
    }

    /** Returns the number of elements in the column. */
    virtual size_t size() {
        assert(false);
        return 0;
    }

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'. */
    virtual char get_type() {
        assert(false);
        return 'U';
    }

    virtual Column* clone() {
        assert(false);
        return nullptr;
    }
};

/*************************************************************************
 * IntColumn::
 * Holds int values.
 */
class IntColumn : public Column {
   public:
    size_t size_;
    size_t capacity_;
    size_t num_segments_;
    size_t segments_capacity_;
    int** segments_;

    IntColumn() : Column() {
        size_ = 0;
        capacity_ = SEGMENT_SIZE;
        num_segments_ = 1;
        segments_capacity_ = 10;
        segments_ = new int*[segments_capacity_];
        segments_[0] = new int[SEGMENT_SIZE];
    }

    IntColumn(int n, ...) : IntColumn() {
        va_list args;
        va_start(args, n);
        int val = 0;
        for (int i = 0; i < n; i++) {
            val = va_arg(args, int);
            push_back(val);
        }
        va_end(args);
    }

    virtual ~IntColumn() {
        for (size_t i = 0; i < num_segments_; i++) {
            delete[] segments_[i];
        }
        delete[] segments_;
    }

    void expand_() {
        if (num_segments_ == segments_capacity_) {
            size_t new_capacity = num_segments_ * 2;
            int** new_segments = new int*[new_capacity];
            for (size_t i = 0; i < num_segments_; i++) {
                new_segments[i] = segments_[i];
            }

            delete[] segments_;
            segments_ = new_segments;
            segments_capacity_ = new_capacity;
        }

        segments_[num_segments_] = new int[SEGMENT_SIZE];
        capacity_ += SEGMENT_SIZE;
        num_segments_ += 1;
    }

    void push_back(int val) {
        if (size() == capacity_) {
            expand_();
        }
        segments_[num_segments_ - 1][size_ % SEGMENT_SIZE] = val;
        size_ += 1;
    }

    int get(size_t idx) {
        assert(idx < size());
        return segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE];
    }

    IntColumn* as_int() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, int val) {
        assert(idx < size());
        segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE] = val;
    }

    virtual size_t size() {
        return size_;
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
};

/*************************************************************************
 * BoolColumn::
 * Holds bool values.
 */
class BoolColumn : public Column {
   public:
    size_t size_;
    size_t capacity_;
    size_t num_segments_;
    size_t segments_capacity_;
    bool** segments_;

    BoolColumn() : Column() {
        size_ = 0;
        capacity_ = SEGMENT_SIZE;
        num_segments_ = 1;
        segments_capacity_ = 10;
        segments_ = new bool*[segments_capacity_];
        segments_[0] = new bool[SEGMENT_SIZE];
    }

    BoolColumn(int n, ...) : BoolColumn() {
        va_list args;
        va_start(args, n);
        bool val = false;
        for (int i = 0; i < n; i++) {
            val = va_arg(args, bool);
            push_back(val);
        }
        va_end(args);
    }

    virtual ~BoolColumn() {
        for (size_t i = 0; i < num_segments_; i++) {
            delete[] segments_[i];
        }
        delete[] segments_;
    }

    void expand_() {
        if (num_segments_ == segments_capacity_) {
            size_t new_capacity = num_segments_ * 2;
            bool** new_segments = new bool*[new_capacity];
            for (size_t i = 0; i < num_segments_; i++) {
                new_segments[i] = segments_[i];
            }

            delete[] segments_;
            segments_ = new_segments;
            segments_capacity_ = new_capacity;
        }

        segments_[num_segments_] = new bool[SEGMENT_SIZE];
        capacity_ += SEGMENT_SIZE;
        num_segments_ += 1;
    }

    void push_back(bool val) {
        if (size() == capacity_) {
            expand_();
        }
        segments_[num_segments_ - 1][size_ % SEGMENT_SIZE] = val;
        size_ += 1;
    }

    bool get(size_t idx) {
        assert(idx < size());
        return segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE];
    }

    BoolColumn* as_bool() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, bool val) {
        assert(idx < size());
        segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE] = val;
    }

    virtual size_t size() {
        return size_;
    }

    virtual char get_type() {
        return 'B';
    }

    virtual Column* clone() {
        BoolColumn* result = new BoolColumn();
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }
};

/*************************************************************************
 * FloatColumn::
 * Holds float values.
 */
class FloatColumn : public Column {
   public:
    size_t size_;
    size_t capacity_;
    size_t num_segments_;
    size_t segments_capacity_;
    float** segments_;

    FloatColumn() : Column() {
        size_ = 0;
        capacity_ = SEGMENT_SIZE;
        num_segments_ = 1;
        segments_capacity_ = 10;
        segments_ = new float*[segments_capacity_];
        segments_[0] = new float[SEGMENT_SIZE];
    }

    FloatColumn(int n, ...) : FloatColumn() {
        va_list args;
        va_start(args, n);
        float val = 0;
        for (int i = 0; i < n; i++) {
            val = va_arg(args, float);
            push_back(val);
        }
        va_end(args);
    }

    virtual ~FloatColumn() {
        for (size_t i = 0; i < num_segments_; i++) {
            delete[] segments_[i];
        }
        delete[] segments_;
    }

    void expand_() {
        if (num_segments_ == segments_capacity_) {
            size_t new_capacity = num_segments_ * 2;
            float** new_segments = new float*[new_capacity];
            for (size_t i = 0; i < num_segments_; i++) {
                new_segments[i] = segments_[i];
            }

            delete[] segments_;
            segments_ = new_segments;
            segments_capacity_ = new_capacity;
        }

        segments_[num_segments_] = new float[SEGMENT_SIZE];
        capacity_ += SEGMENT_SIZE;
        num_segments_ += 1;
    }

    void push_back(float val) {
        if (size() == capacity_) {
            expand_();
        }
        segments_[num_segments_ - 1][size_ % SEGMENT_SIZE] = val;
        size_ += 1;
    }

    float get(size_t idx) {
        assert(idx < size());
        return segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE];
    }

    FloatColumn* as_float() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, float val) {
        assert(idx < size());
        segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE] = val;
    }

    virtual size_t size() {
        return size_;
    }

    virtual char get_type() {
        return 'F';
    }

    virtual Column* clone() {
        FloatColumn* result = new FloatColumn();
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
    size_t size_;
    size_t capacity_;
    size_t num_segments_;
    size_t segments_capacity_;
    String*** segments_;

    StringColumn() : Column() {
        size_ = 0;
        capacity_ = SEGMENT_SIZE;
        num_segments_ = 1;
        segments_capacity_ = 10;
        segments_ = new String**[segments_capacity_];
        segments_[0] = new String*[SEGMENT_SIZE];
    }

    StringColumn(int n, ...) : StringColumn() {
        va_list args;
        va_start(args, n);
        String* val = nullptr;
        for (int i = 0; i < n; i++) {
            val = va_arg(args, String*);
            push_back(val);
        }
        va_end(args);
    }

    virtual ~StringColumn() {
        for (size_t i = 0; i < num_segments_; i++) {
            delete[] segments_[i];
        }
        delete[] segments_;
    }

    void expand_() {
        if (num_segments_ == segments_capacity_) {
            size_t new_capacity = num_segments_ * 2;
            String*** new_segments = new String**[new_capacity];
            for (size_t i = 0; i < num_segments_; i++) {
                new_segments[i] = segments_[i];
            }

            delete[] segments_;
            segments_ = new_segments;
            segments_capacity_ = new_capacity;
        }

        segments_[num_segments_] = new String*[SEGMENT_SIZE];
        capacity_ += SEGMENT_SIZE;
        num_segments_ += 1;
    }

    void push_back(String* val) {
        if (size() == capacity_) {
            expand_();
        }
        segments_[num_segments_ - 1][size_ % SEGMENT_SIZE] = val;
        size_ += 1;
    }

    String* get(size_t idx) {
        assert(idx < size());
        return segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE];
    }

    StringColumn* as_string() {
        return this;
    }

    /** Set value at idx. An out of bound idx is undefined.  */
    void set(size_t idx, String* val) {
        assert(idx < size());
        segments_[idx / SEGMENT_SIZE][idx % SEGMENT_SIZE] = val;
    }

    virtual size_t size() {
        return size_;
    }

    virtual char get_type() {
        return 'S';
    }

    virtual Column* clone() {
        StringColumn* result = new StringColumn();
        for (size_t i = 0; i < size(); i++) {
            result->push_back(get(i));
        }
        return result;
    }
};
