#pragma once
#include "fielder.h"
#include "schema.h"
#include "util/array.h"
#include "util/object.h"
#include "util/string.h"
#include <vector>

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 */
class Row : public Object {
   public:
    int index_ = -1;
    std::vector<Object*> values_;
    std::vector<char> types_;

    /** Build a row following a schema. */
    Row(Schema& scm) : Object() {
        values_ = std::vector<Object*>();
        types_ = std::vector<int>();
        for (size_t i = 0; i < scm.width(); i++) {
            switch (scm.col_type(i)) {
                case 'S':
                    values_.push_back(new StringArray());
                    types_.push_back('S');
                    break;
                case 'B':
                    values_.push_back(new BoolArray());
                    types_.push_back('B');
                    break;
                case 'I':
                    values_.push_back(new IntArray());
                    types_.push_back('I');
                    break;
                case 'D':
                    values_.push_back(new DoubleArray());
                    types_.push_back('D');
                    break;
                default:
                    assert(false);
            }
        }
    }

    virtual ~Row() {
        for (size_t i = 0; i < values_.size(); i++) {
            delete values_[i];
        }
    }

    /** Setters: set the given column with the given value. Setting a column with
     * a value of the wrong type is undefined. */
    void set(size_t col, int val) {
        assert(col < values_.size());
        assert(types_[col] == 'I');
        IntArray* c = static_cast<IntArray*>(values_[col]);
        assert(c != nullptr);
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }
    void set(size_t col, double val) {
        assert(col < values_.size());
        assert(types_[col] == 'D');
        DoubleArray* c = static_cast<DoubleArray*>(values_[col]);
        assert(c != nullptr);
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }
    void set(size_t col, bool val) {
        assert(col < values_.size());
        assert(types_[col] == 'B');
        BoolArray* c = static_cast<BoolArray*>(values_[col]);
        assert(c != nullptr);
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }

    /** The string is external. */
    void set(size_t col, String* val) {
        assert(col < values_.size());
        assert(types_[col] == 'S');
        StringArray* c = static_cast<StringArray*>(values_[col]);
        assert(c != nullptr);
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }

    /** Set/get the index of this row (ie. its position in the dataframe. This is
     *  only used for informational purposes, unused otherwise */
    void set_idx(size_t idx) {
        index_ = idx;
    }
    size_t get_idx() {
        return index_;
    }

    /** Getters: get the value at the given column. If the column is not
     * of the requested type, the result is undefined. */
    int get_int(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'I');
        return static_cast<IntArray*>(values_[col])->get(0);
    }

    bool get_bool(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'B');
        return static_cast<BoolArray*>(values_[col])->get(0);
    }

    double get_double(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'D');
        return static_cast<DoubleArray*>(values_[col])->get(0);
    }

    String* get_string(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'S');
        return static_cast<StringArray*>(values_[col])->get(0);
    }

    /** Number of fields in the row. */
    size_t width() {
        return types_.size();
    }

    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) {
        assert(idx < types_.size());
        return types_[idx];
    }

    /** Given a Fielder, visit every field of this row. The first argument is
     * index of the row in the dataframe.
     * Calling this method before the row's fields have been set is undefined. */
    void visit(size_t idx, Fielder& f) {
        assert(idx < values_.size());
        switch (col_type(idx)) {
            case 'S':
                f.accept(get_string(idx));
                break;
            case 'I':
                f.accept(get_int(idx));
                break;
            case 'D':
                f.accept(get_double(idx));
                break;
            case 'B':
                f.accept(get_bool(idx));
                break;
            default:
                assert(false);
        }
    }
};
