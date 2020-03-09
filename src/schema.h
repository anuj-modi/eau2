#pragma once
#include <stdio.h>
#include "array.h"
#include "string.h"

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object {
    Array* col_names_;
    IntArray* col_types_;
    Array* row_names_;

   public:
    /** Copying constructor */
    Schema(Schema& from) : Object() {
        col_names_ = from.col_names_->clone();
        col_types_ = from.col_types_->clone();
        row_names_ = from.row_names_->clone();
    }

    /** Create an empty schema **/
    Schema() : Object() {
        col_names_ = new Array();
        col_types_ = new IntArray();
        row_names_ = new Array();
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
            col_names_->push_back(nullptr);
        }
    }

    virtual ~Schema() {
        delete col_names_;
        delete col_types_;
        delete row_names_;
    }

    /** Add a column of the given type and name (can be nullptr), name
     * is external. Names are expectd to be unique, duplicates result
     * in undefined behavior. */
    void add_column(char type, String* name) {
        assert(!col_names_->contains(name));
        col_types_->push_back(type_to_int_(type));
        col_names_->push_back(name);
    }

    /** Add a row with a name (possibly nullptr), name is external.  Names are
     *  expectd to be unique, duplicates result in undefined behavior. */
    void add_row(String* name) {
        assert(!row_names_->contains(name));
        row_names_->push_back(name);
    }

    /** Return name of row at idx; nullptr indicates no name. An idx >= width
     * is undefined. */
    String* row_name(size_t idx) {
        assert(idx < row_names_->size());
        if (row_names_->get(idx) == nullptr) {
            return nullptr;
        } else {
            return static_cast<String*>(row_names_->get(idx));
        }
    }

    /** Return name of column at idx; nullptr indicates no name given.
     *  An idx >= width is undefined.*/
    String* col_name(size_t idx) {
        assert(idx < col_names_->size());
        if (col_names_->get(idx) == nullptr) {
            return nullptr;
        } else {
            return static_cast<String*>(col_names_->get(idx));
        }
    }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        assert(idx < col_types_->size());
        return type_to_char_(col_types_->get(idx));
    }

    /** Given a column name return its index, or -1. */
    int col_idx(const char* name) {
        if (name == nullptr) {
            return -1;
        }
        String* n = new String(name);
        int result = -1;
        if (col_names_->contains(n)) {
            result = col_names_->index_of(n);
        }

        delete n;
        return result;
    }

    /** Given a row name return its index, or -1. */
    int row_idx(const char* name) {
        if (name == nullptr) {
            return -1;
        }
        String* n = new String(name, strlen(name));
        if (row_names_->contains(n)) {
            return row_names_->index_of(n);
        } else {
            return -1;
        }
        delete n;
    }

    /** The number of columns */
    size_t width() {
        return col_names_->size();
    }

    /** The number of rows */
    size_t length() {
        return row_names_->size();
    }

    int type_to_int_(char c) {
        switch (c) {
            case 'S':
                return 1;
            case 'B':
                return 2;
            case 'I':
                return 3;
            case 'F':
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
                return 'F';
            default:
                fprintf(stderr, "Invalid column type given: %d\n", i);
                assert(false);
        }
    }
};
