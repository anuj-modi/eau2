#pragma once
#include <vector>

#include "schema.h"
#include "util/array.h"
#include "util/object.h"
#include "util/string.h"

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
    std::vector<Data> values_;
    std::vector<char> types_;

    /** Build a row following a schema. */
    Row(Schema& scm) : Object() {
        values_ = std::vector<Object*>();
        types_ = std::vector<char>();
        for (size_t i = 0; i < scm.width(); i++) {
            switch (scm.col_type(i)) {
                case 'S':
                    types_.push_back('S');
                    break;
                case 'B':
                    types_.push_back('B');
                    break;
                case 'I':
                    types_.push_back('I');
                    break;
                case 'D':
                    types_.push_back('D');
                    break;
                default:
                    assert(false);
            }
        }
    }

    virtual ~Row() {}

    /** Getters: get the value at the given column. If the column is not
     * of the requested type, the result is undefined. */
    int get_int(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'I');
        return values_[col].payload.i;
    }

    bool get_bool(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'B');
        return values_[col].payload.b;
    }

    double get_double(size_t col) {
        assert(col < values_.size());
        assert(types_[col] == 'D');
        return values_[col].payload.d;
    }

    String* get_string(size_t col) {
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

    /** Given a Fielder, visit every field of this row. The first argument is
     * index of the row in the dataframe.
     * Calling this method before the row's fields have been set is undefined. */
    void visit(size_t idx, Visitor& v) {
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