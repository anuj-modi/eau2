#pragma once
#include <assert.h>
#include <vector>
#include "column.h"
#include "schema.h"
#include "store/key.h"
#include "visitor.h"
#include "row.h"

class KDStore;

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 * Author: gomes.chri, modi.an
 */
class DataFrame : public Object {
   public:
    std::vector<Column*> columns_;
    Schema* df_schema_;
    KVStore* store_;

    /**
     * Creates a data frame from a given set of columns.
     * Data frame takes ownership of the given columns.
     */
    DataFrame(std::vector<Column*> columns, KVStore* store) : Object() {
        size_t length = columns[0]->size();
        for (size_t i = 1; i < columns.size(); i++) {
            assert(columns[i]->size() == length);
        }
        store_ = store;
        df_schema_ = new Schema();
        df_schema_->add_rows(length);
        columns_ = std::vector<Column*>();
        for (size_t i = 0; i < columns.size(); i++) {
            add_column_(columns[i]);
        }
    }
    
    /**
     * Creates a data frame from a column.
     * Data frame takes ownership of the column.
     */
    DataFrame(Column* c, KVStore* store) : Object() {
        assert(c != nullptr && store != nullptr);
        store_ = store;
        df_schema_ = new Schema();
        df_schema_->add_rows(c->size());
        add_column_(c);
    }

    /**
     * Creates a data frame from the given deserializer.
     */
    DataFrame(Deserializer* d, KVStore* store) : Object() {
        df_schema_ = new Schema(d);
        store_ = store;
        for (size_t i = 0; i < df_schema_->width(); i++) {
            switch (df_schema_->col_type(i)) {
                case 'S':
                    columns_.push_back(new StringColumn(store, d));
                    break;
                case 'I':
                    columns_.push_back(new IntColumn(store, d));
                    break;
                case 'B':
                    columns_.push_back(new BoolColumn(store, d));
                    break;
                case 'D':
                    columns_.push_back(new DoubleColumn(store, d));
                    break;
                default:
                    assert(false);
            }
        }
    }

    virtual ~DataFrame() {
        for (size_t i = 0; i < columns_.size(); i++) {
            delete columns_[i];
        }
        delete df_schema_;
    }

    /** Returns the dataframe's schema. Modifying the schema after a dataframe
     * has been created in undefined. */
    Schema& get_schema() {
        return *df_schema_;
    }

    /** Return the value at the given column and row. Accessing rows or
     *  columns out of bounds, or request the wrong type is undefined.*/
    int get_int(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'I');
        return columns_[col]->as_int()->get(row);
    }
    bool get_bool(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'B');
        return columns_[col]->as_bool()->get(row);
    }
    double get_double(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'D');
        return columns_[col]->as_double()->get(row);
    }
    String* get_string(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'S');
        return columns_[col]->as_string()->get(row);
    }

    /**
     * Fill the row given with data in the data frame.
     * @arg idx  the row index in the data frame
     * @arg r  the row
     */
    // void fill_row(size_t idx, Row& row) {
    //     assert(idx < df_schema_->length());
    //     assert(row.width() == df_schema_->width());
    //     for (size_t i = 0; i < row.width(); i++) {
    //         assert(df_schema_->col_type(i) == row.col_type(i));
    //     }
    //     for (size_t j = 0; j < row.width(); j++) {
    //         char c_type = row.col_type(j);
    //         switch (c_type) {
    //             case 'S':
    //                 row.set(j, get_string(j, idx));
    //                 break;
    //             case 'B':
    //                 row.set(j, get_bool(j, idx));
    //                 break;
    //             case 'I':
    //                 row.set(j, get_int(j, idx));
    //                 break;
    //             case 'D':
    //                 row.set(j, get_double(j, idx));
    //                 break;
    //             default:
    //                 assert(false);
    //         }
    //     }
    // }

    /** The number of rows in the dataframe. */
    size_t nrows() {
        return df_schema_->length();
    }

    /** The number of columns in the dataframe.*/
    size_t ncols() {
        return df_schema_->width();
    }

    /** Get the type of a column in a dataframe. */
    char col_type(size_t col_idx) {
        return df_schema_->col_type(col_idx);
    }

    void serialize(Serializer* s) {
        df_schema_->serialize(s);
        for (size_t i = 0; i < columns_.size(); i++) {
            columns_[i]->serialize(s);
        }
    }
    
    /** Adds a column this dataframe, updates the schema, the new column
     * is external, and appears as the last column of the dataframe.
     * A nullptr colum is undefined. */
    void add_column_(Column* col) {
        assert(col != nullptr);
        assert(col->size() == df_schema_->length());
        df_schema_->add_column(col->get_type());
        col->finalize();
        columns_.push_back(col);
    }

    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, double* vals);
    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, int* vals);
    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, bool* vals);
    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, String** vals);

    static DataFrame* fromScalar(Key* k, KDStore* kd, double val);
    static DataFrame* fromScalar(Key* k, KDStore* kd, int val);
    static DataFrame* fromScalar(Key* k, KDStore* kd, bool val);
    static DataFrame* fromScalar(Key* k, KDStore* kd, String* val);

    static DataFrame* fromVisitor(Key* k, KDStore* kd, const char* types, Writer v);
};
