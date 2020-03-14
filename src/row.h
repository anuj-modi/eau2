#pragma once
#include "array.h"
#include "fielder.h"
#include "object.h"
#include "schema.h"
#include "string.h"

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
    Array* values_;
    IntArray* types_;

    /** Build a row following a schema. */
    Row(Schema& scm) : Object() {
        values_ = new Array();
        types_ = new IntArray();
        for (size_t i = 0; i < scm.width(); i++) {
            switch (scm.col_type(i)) {
                case 'S':
                    values_->push_back(new StringColumn());
                    types_->push_back(type_to_int_('S'));
                    break;
                case 'B':
                    values_->push_back(new BoolColumn());
                    types_->push_back(type_to_int_('B'));
                    break;
                case 'I':
                    values_->push_back(new IntColumn());
                    types_->push_back(type_to_int_('I'));
                    break;
                case 'D':
                    values_->push_back(new DoubleColumn());
                    types_->push_back(type_to_int_('D'));
                    break;
                default:
                    assert(false);
            }
        }
    }

    virtual ~Row() {
        for (size_t i = 0; i < values_->size(); i++) {
            delete values_->get(i);
        }
        delete values_;
        delete types_;
    }

    /** Setters: set the given column with the given value. Setting a column with
     * a value of the wrong type is undefined. */
    void set(size_t col, int val) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'I');
        IntColumn* c = static_cast<Column*>(values_->get(col))->as_int();
        assert(c != nullptr);
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }
    void set(size_t col, double val) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'D');
        DoubleColumn* c = static_cast<Column*>(values_->get(col))->as_double();
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }
    void set(size_t col, bool val) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'B');
        BoolColumn* c = static_cast<Column*>(values_->get(col))->as_bool();
        if (c->size() == 0) {
            c->push_back(val);
        } else {
            c->set(0, val);
        }
    }

    /** The string is external. */
    void set(size_t col, String* val) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'S');
        StringColumn* c = static_cast<Column*>(values_->get(col))->as_string();
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
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'I');
        return static_cast<Column*>(values_->get(col))->as_int()->get(0);
    }

    bool get_bool(size_t col) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'B');
        return static_cast<Column*>(values_->get(col))->as_bool()->get(0);
    }

    double get_double(size_t col) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'D');
        return static_cast<Column*>(values_->get(col))->as_double()->get(0);
    }

    String* get_string(size_t col) {
        assert(col < values_->size());
        assert(type_to_char_(types_->get(col)) == 'S');
        return static_cast<Column*>(values_->get(col))->as_string()->get(0);
    }

    /** Number of fields in the row. */
    size_t width() {
        return types_->size();
    }

    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) {
        assert(idx < types_->size());
        return type_to_char_(types_->get(idx));
    }

    /** Given a Fielder, visit every field of this row. The first argument is
     * index of the row in the dataframe.
     * Calling this method before the row's fields have been set is undefined. */
    void visit(size_t idx, Fielder& f) {
        assert(idx < values_->size());
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

    int type_to_int_(char c) {
        switch (c) {
            case 'S':
                return 1;
            case 'B':
                return 2;
            case 'I':
                return 3;
            case 'D':
                return 4;
            default:
                printf("Invalid column type given");
                assert(false);
        }
    }

    char type_to_char_(int i) {
        switch (i) {
            case 1:
                return 'S';
            case 2:
                return 'B';
            case 3:
                return 'I';
            case 4:
                return 'D';
            default:
                printf("Invalid column type given");
                assert(false);
        }
    }
};
