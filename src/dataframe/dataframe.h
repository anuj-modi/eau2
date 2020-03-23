#pragma once
#include <vector>
#include "dataframe_base.h"
#include "store/key.h"
#include "store/kdstore.h"
#include "worker.h"

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 */
class DataFrame : public DataFrameBase {
   public:
    /** Create a data frame with the same columns as the given df but with no
     * rows or rownmaes */
    DataFrame(DataFrame& df) : DataFrameBase(df) {}

    /** Create a data frame from a schema and columns. All columns are created
     * empty. */
    DataFrame(Schema& schema) : DataFrameBase(schema) {}

    /**
     * Creates a data frame from a given set of columns.
     */
    DataFrame(std::vector<Column*> columns) : DataFrameBase(columns) {}

    /**
     * Creates a data frame from the given deserializer.
     */
    DataFrame(Deserializer* d) : DataFrameBase(d) {}

    virtual ~DataFrame() {}

    static DataFrame* putDataFrame(Key* k, KDStore* kd, DataFrame* df) {
        Serializer serializer;
        df->serialize(&serializer);
        Value v(serializer.get_bytes(), serializer.size());
        kd->put(k, &v);
        return df;
    }

    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, double* vals) {
        Schema s("D");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        for (size_t i = 0; i < size; i++) {
            r.set(0, vals[i]);
            df->add_row(r);
        }
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, int* vals) {
        Schema s("I");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        for (size_t i = 0; i < size; i++) {
            r.set(0, vals[i]);
            df->add_row(r);
        }
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, bool* vals) {
        Schema s("B");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        for (size_t i = 0; i < size; i++) 
            r.set(0, vals[i]);
            df->add_row(r);
        }
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromArray(Key* k, KDStore* kd, size_t size, String** vals) {
        Schema s("S");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        for (size_t i = 0; i < size; i++) {
            r.set(0, vals[i]);
            df->add_row(r);
        }
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromScalar(Key* k, KDStore* kd, size_t size, double val) {
        Schema s("D");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        r.set(0, val);
        df->add_row(r);
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromScalar(Key* k, KDStore* kd, size_t size, int val) {
        Schema s("I");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        r.set(0, val);
        df->add_row(r);
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromScalar(Key* k, KDStore* kd, size_t size, bool val) {
        Schema s("B");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        r.set(0, val);
        df->add_row(r);
        return putDataFrame(k, kd, df);
    }

    static DataFrame* fromScalar(Key* k, KDStore* kd, size_t size, String* val) {
        Schema s("S");
        Row r(s);
        DataFrame* df = new DataFrame(s);
        r.set(0, val);
        df->add_row(r);
        return putDataFrame(k, kd, df);
    }

    /** Create a new dataframe, constructed from rows for which the given
     * Rower returned true from its accept method. */
    DataFrame* filter(Rower& r) {
        DataFrame* new_df = new DataFrame(*df_schema_);
        for (size_t i = 0; i < df_schema_->length(); i++) {
            Row curr_row = Row(*df_schema_);
            fill_row(i, curr_row);
            if (r.accept(curr_row)) {
                new_df->add_row(curr_row);
            }
        }
        return new_df;
    }

    /** This method clones the Rower and executes the map in parallel. Join is
     * used at the end to merge the results. */
    void pmap(Rower& r) {
        size_t num_workers = std::thread::hardware_concurrency();
        Worker** workers = new Worker*[num_workers];
        size_t chunk_size = nrows() / num_workers;
        size_t leftovers = nrows() % num_workers;
        for (size_t i = 0; i < num_workers; i++) {
            if (i == num_workers - 1) {
                workers[i] = new Worker(&r, this, i, chunk_size, leftovers);
            } else {
                workers[i] = new Worker(&r, this, i, chunk_size);
            }
            workers[i]->start();
        }

        for (size_t i = 0; i < num_workers; i++) {
            workers[i]->join();
            r.join_delete(workers[i]->rower);
            delete workers[i];
        }

        delete[] workers;
    }
};
