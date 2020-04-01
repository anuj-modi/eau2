#pragma once
#include <vector>

#include "schema.h"
#include "util/object.h"
#include "util/string.h"
#include "visitor.h"
#include "data.h"

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows.
 * Author: gomes.chri and modi.an
 */
class Row : public Object {
   public:
    std::vector<Data> values_;
    Schema s_;

    /** Build a row following a schema. */
    Row(Schema& scm) : Object() {
        values_ = std::vector<Data>();
        s_ = Schema(scm);
        for (size_t i = 0; i < s_.width(); i++) {
            values_.push_back(Data());
        }
    }

    virtual ~Row() {}

    /** Getters: get the value at the given column. If the column is not
     * of the requested type, the result is undefined.
     * Strings returned are borrowed.
     * Calling get on a column before setting it is undefined behavior.
     */
    int get_int(size_t col) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'I');
        return values_[col].payload.i;
    }

    bool get_bool(size_t col) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'B');
        return values_[col].payload.b;
    }

    double get_double(size_t col) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'D');
        return values_[col].payload.d;
    }

    String* get_string(size_t col) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'S');
        return values_[col].payload.s;
    }

    /** Setters: set the value at the given column. If the column is not
     * of the requested type, the result is undefined.
     * Strings are external to the row (borrowed by row).
     */
    void set(size_t col, int i) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'I');
        values_[col].payload.i = i;
    }

    void set(size_t col, bool b) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'B');
        values_[col].payload.b = b;
    }

    void set(size_t col, double d) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'D');
        values_[col].payload.d = d;
    }

    void set(size_t col, String* s) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'S');
        values_[col].payload.s = s;
    }

    /** Number of fields in the row. */
    size_t width() {
        return s_.width();
    }

    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) {
        assert(idx < s_.width());
        return s_.col_type(idx);
    }
};