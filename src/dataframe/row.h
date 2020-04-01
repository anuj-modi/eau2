#pragma once
#include <vector>

#include "column.h"
#include "schema.h"
#include "util/data.h"
#include "util/object.h"
#include "util/string.h"
#include "visitor.h"

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
            values_[i].payload.s = nullptr;
        }
    }

    virtual ~Row() {
        for (size_t i = 0; i < s_.width(); i++) {
            if (values_[i].payload.s != nullptr && s_.col_type(i) == 'S') {   
                delete values_[i].payload.s;
            }
        }
    }

    /** Getters: get the value at the given column. If the column is not
     * of the requested type, the result is undefined.
     * Strings returned are borrowed.
     * Calling get on a column before setting it is undefined behavior.
     * @arg col  the index of the col
     * @return  the value at the index
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
     * Strings are consumed by the row.
     * @arg col  index of the col
     * @arg v  the value to add
     */
    void set(size_t col, int v) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'I');
        values_[col].payload.i = v;
    }

    void set(size_t col, bool v) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'B');
        values_[col].payload.b = v;
    }

    void set(size_t col, double v) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'D');
        values_[col].payload.d = v;
    }

    void set(size_t col, String* v) {
        assert(col < s_.width());
        assert(s_.col_type(col) == 'S');
        if (values_[col].payload.s != nullptr) {
            delete values_[col].payload.s;
        }
        values_[col].payload.s = v;
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

    /**
     * Adds the row contents to the given columns.
     * Adds in order of the row's schema.
     * @arg cols  the columns to add to
     */
    void add_to_columns(std::vector<Column*> cols) {
        assert(s_.width() == cols.size());
        for (size_t i = 0; i < s_.width(); i++) {
            assert(s_.col_type(i) == cols[i]->get_type());
        }
        for (size_t j = 0; j < s_.width(); j++) {
            switch (s_.col_type(j)) {
                case 'S':
                    cols[j]->push_back(values_[j].payload.s);
                    break;
                case 'I':
                    cols[j]->push_back(values_[j].payload.i);
                    break;
                case 'D':
                    cols[j]->push_back(values_[j].payload.d);
                    break;
                case 'B':
                    cols[j]->push_back(values_[j].payload.b);
                    break;
                default:
                    assert(false);
            }
        }
    }
};