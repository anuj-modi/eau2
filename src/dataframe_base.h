#pragma once
#include <assert.h>
#include "column.h"
#include "row.h"
#include "rower.h"
#include "schema.h"

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it. This is a data frame with only sequential map.
 */
class DataFrameBase : public Object {
   public:
    Array* columns_;
    Schema* df_schema_;

    /** Create a data frame with the same columns as the given df but with no
     * rows or rownmaes */
    DataFrameBase(DataFrameBase& df) : DataFrameBase(df.get_schema()) {}

    /** Create a data frame from a schema and columns. All columns are created
     * empty. */
    DataFrameBase(Schema& schema) : Object() {
        df_schema_ = new Schema();
        columns_ = new Array();
        for (size_t i = 0; i < schema.width(); i++) {
            switch (schema.col_type(i)) {
                case 'S':
                    df_schema_->add_column('S');
                    columns_->push_back(new StringColumn());
                    break;
                case 'I':
                    df_schema_->add_column('I');
                    columns_->push_back(new IntColumn());
                    break;
                case 'B':
                    df_schema_->add_column('B');
                    columns_->push_back(new BoolColumn());
                    break;
                case 'F':
                    df_schema_->add_column('F');
                    columns_->push_back(new DoubleColumn());
                    break;
                default:
                    assert(false);
            }
        }
    }

    virtual ~DataFrameBase() {
        for (size_t i = 0; i < columns_->size(); i++) {
            delete columns_->get(i);
        }
        delete df_schema_;
        delete columns_;
    }

    /** Returns the dataframe's schema. Modifying the schema after a dataframe
     * has been created in undefined. */
    Schema& get_schema() {
        return *df_schema_;
    }

    /** Adds a column this dataframe, updates the schema, the new column
     * is external, and appears as the last column of the dataframe.
     * A nullptr colum is undefined. */
    void add_column(Column* col) {
        assert(col != nullptr);
        if (col->size() == df_schema_->length()) {
            df_schema_->add_column(col->get_type());
            columns_->push_back(col->clone());
        }
    }

    /** Return the value at the given column and row. Accessing rows or
     *  columns out of bounds, or request the wrong type is undefined.*/
    int get_int(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'I');
        return static_cast<Column*>(columns_->get(col))->as_int()->get(row);
    }
    bool get_bool(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'B');
        return static_cast<Column*>(columns_->get(col))->as_bool()->get(row);
    }
    double get_double(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'F');
        return static_cast<Column*>(columns_->get(col))->as_double()->get(row);
    }
    String* get_string(size_t col, size_t row) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'S');
        return static_cast<Column*>(columns_->get(col))->as_string()->get(row);
    }

    /** Set the value at the given column and row to the given value.
     * If the column is not  of the right type or the indices are out of
     * bound, the result is undefined. */
    void set(size_t col, size_t row, int val) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'I');
        static_cast<Column*>(columns_->get(col))->as_int()->set(row, val);
    }
    void set(size_t col, size_t row, bool val) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'B');
        static_cast<Column*>(columns_->get(col))->as_bool()->set(row, val);
    }
    void set(size_t col, size_t row, double val) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'F');
        static_cast<Column*>(columns_->get(col))->as_double()->set(row, val);
    }
    void set(size_t col, size_t row, String* val) {
        assert(col < df_schema_->width() && row < df_schema_->length());
        assert(df_schema_->col_type(col) == 'S');
        static_cast<Column*>(columns_->get(col))->as_string()->set(row, val);
    }

    /** Set the fields of the given row object with values from the columns at
     * the given offset.  If the row is not form the same schema as the
     * dataframe, results are undefined.
     */
    void fill_row(size_t idx, Row& row) {
        assert(idx < df_schema_->length());
        assert(row.width() == df_schema_->width());
        for (size_t i = 0; i < row.width(); i++) {
            assert(df_schema_->col_type(i) == row.col_type(i));
        }
        row.set_idx(idx);
        for (size_t j = 0; j < row.width(); j++) {
            char c_type = row.col_type(j);
            switch (c_type) {
                case 'S':
                    row.set(j, static_cast<Column*>(columns_->get(j))->as_string()->get(idx));
                    break;
                case 'B':
                    row.set(j, static_cast<Column*>(columns_->get(j))->as_bool()->get(idx));
                    break;
                case 'I':
                    row.set(j, static_cast<Column*>(columns_->get(j))->as_int()->get(idx));
                    break;
                case 'F':
                    row.set(j, static_cast<Column*>(columns_->get(j))->as_double()->get(idx));
                    break;
                default:
                    assert(false);
            }
        }
    }

    /** Add a row at the end of this dataframe. The row is expected to have
     *  the right schema and be filled with values, otherwise undedined.  */
    void add_row(Row& row) {
        assert(row.width() == df_schema_->width());
        for (size_t i = 0; i < row.width(); i++) {
            assert(df_schema_->col_type(i) == row.col_type(i));
        }
        for (size_t j = 0; j < row.width(); j++) {
            char c_type = row.col_type(j);
            switch (c_type) {
                case 'S':
                    static_cast<Column*>(columns_->get(j))->push_back(row.get_string(j));
                    break;
                case 'B':
                    static_cast<Column*>(columns_->get(j))->push_back(row.get_bool(j));
                    break;
                case 'I':
                    static_cast<Column*>(columns_->get(j))->push_back(row.get_int(j));
                    break;
                case 'F':
                    static_cast<Column*>(columns_->get(j))->push_back(row.get_double(j));
                    break;
                default:
                    assert(false);
            }
        }
        df_schema_->add_row();
    }

    /** The number of rows in the dataframe. */
    size_t nrows() {
        return df_schema_->length();
    }

    /** The number of columns in the dataframe.*/
    size_t ncols() {
        return df_schema_->width();
    }

    /** Visit rows in order */
    void map(Rower& r) {
        for (size_t i = 0; i < df_schema_->length(); i++) {
            Row curr_row = Row(*df_schema_);
            fill_row(i, curr_row);
            r.accept(curr_row);
        }
    }

    /** Create a new dataframe, constructed from rows for which the given
     * Rower returned true from its accept method. */
    virtual DataFrameBase* filter(Rower& r) {
        DataFrameBase* new_df = new DataFrameBase(*df_schema_);
        for (size_t i = 0; i < df_schema_->length(); i++) {
            Row curr_row = Row(*df_schema_);
            fill_row(i, curr_row);
            if (r.accept(curr_row)) {
                new_df->add_row(curr_row);
            }
        }
        return new_df;
    }

    /** Print the dataframe in SoR format to standard output. */
    void print() {
        PrintRower pr = PrintRower();
        map(pr);
    }
};
