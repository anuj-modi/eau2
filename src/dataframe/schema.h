#pragma once
#include <stdio.h>
#include <vector>
#include "util/array.h"
#include "util/serial.h"
#include "util/string.h"

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'D'.
 */
class Schema : public Object {
    std::vector<char> col_types_;
    size_t num_rows_;

   public:
    /** Copying constructor */
    Schema(Schema& from) : Object() {
        col_types_ = std::vector<char>(from.col_types_);
        num_rows_ = from.num_rows_;
    }

    /** Create an empty schema **/
    Schema() : Object() {
        col_types_ = std::vector<char>();
        num_rows_ = 0;
    }

    /** Create a schema from a string of types. A string that contains
     * characters other than those identifying the four type results in
     * undefined behavior. The argument is external, a nullptr argument is
     * undefined. **/
    Schema(const char* types) : Schema() {
        assert(types != nullptr);
        int num_types = strlen(types);
        for (int i = 0; i < num_types; i++) {
            col_types_.push_back(types[i]);
        }
    }

    Schema(Deserializer* d) : Object() {
        col_types_ = std::vector<char>();
        size_t num_cols = d->get_size_t();
        for(size_t i = 0; i < num_cols; i++) {
            col_types_.push_back(d->get_int());
        }
        num_rows_ = d->get_size_t();
    }

    virtual ~Schema() {}

    void add_column(char type) {
        col_types_.push_back(type);
    }

    /** Add a row to the schema. */
    void add_row() {
        add_rows(1);
    }

    /**
     * Adds given number of rows to the schema.
     * @arg n  number of rows to add
     */
    void add_rows(size_t n) {
        num_rows_ += n;
    }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        assert(idx < col_types_.size());
        return col_types_[idx];
    }

    /** The number of columns */
    size_t width() {
        return col_types_.size();
    }

    /** The number of rows */
    size_t length() {
        return num_rows_;
    }

    void serialize(Serializer* s) {
        s->add_size_t(width());
        for(size_t i = 0; i < width(); i++) {
            s->add_int(col_types_[i]);
        }
        s->add_size_t(num_rows_);
    }
};
