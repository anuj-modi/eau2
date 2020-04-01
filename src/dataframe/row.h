#pragma once
#include <vector>

#include "schema.h"
#include "util/array.h"
#include "util/object.h"
#include "util/string.h"
#include "visitor.h"

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
    std::vector<Data> values_;
    Schema s_;

    /** Build a row following a schema. */
    Row(Schema& scm) : Object() {
        values_ = std::vector<Data>();
        s_ = Schema(scm);
    }

    virtual ~Row() {}

    /** Getters: get the value at the given column. If the column is not
     * of the requested type, the result is undefined. */
    int get_int(size_t col) {
        assert(col < values_.size());
        assert(s_.col_type(col) == 'I');
        return values_[col].payload.i;
    }

    bool get_bool(size_t col) {
        assert(col < values_.size());
        assert(s_.col_type(col) == 'B');
        return values_[col].payload.b;
    }

    double get_double(size_t col) {
        assert(col < values_.size());
        assert(s_.col_type(col) == 'D');
        return values_[col].payload.d;
    }

    String* get_string(size_t col) {
        assert(col < values_.size());
        assert(s_.col_type(col) == 'S');
        return values_[col].payload.s;
    }

    /** Setters: set the value at the given column. If the column is not
     * of the requested type, the result is undefined. */
    void set(size_t col, int i) {
        assert(col < values_.size());
        assert(types_[col] == 'I');
        values_[col].payload.i = 
    }

    void get_bool(size_t col, bool b) {
        assert(col < values_.size());
        assert(types_[col] == 'B');
        return values_[col].payload.b;
    }

    void get_double(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'D');
        return values_[col].payload.d;
    }

    void get_string(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'S');
        return values_[col].payload.s;
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
};