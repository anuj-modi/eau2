#pragma once
#include <stdio.h>
#include "array.h"
#include "string.h"

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'D'.
 */
class Schema : public Object {
    IntArray* col_types_;
    size_t num_rows_;

   public:
    /** Copying constructor */
    Schema(Schema& from) : Object() {
        col_types_ = from.col_types_->clone();
        num_rows_ = from.num_rows_;
    }

    /** Create an empty schema **/
    Schema() : Object() {
        col_types_ = new IntArray();
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
            col_types_->push_back(type_to_int_(types[i]));
        }
    }

    virtual ~Schema() {
        delete col_types_;
    }

    void add_column(char type) {
        col_types_->push_back(type_to_int_(type));
    }

    /** Add a row to the schema. */
    void add_row() {
        num_rows_ += 1;
    }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        assert(idx < col_types_->size());
        return type_to_char_(col_types_->get(idx));
    }

    /** The number of columns */
    size_t width() {
        return col_types_->size();
    }

    /** The number of rows */
    size_t length() {
        return num_rows_;
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
                fprintf(stderr, "Invalid column type given\n");
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
                fprintf(stderr, "Invalid column type given: %d\n", i);
                assert(false);
        }
    }
};
