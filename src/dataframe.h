#pragma once
#include "dataframe_base.h"
#include "thread.h"
#include "worker.h"

static const int NUM_WORKERS = 4;

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

    /** This method clones the Rower and executes the map in parallel. Join is
     * used at the end to merge the results. */
    void pmap(Rower& r) {
        Worker** workers = new Worker*[NUM_WORKERS];
        size_t chunk_size = nrows() / NUM_WORKERS;
        for (size_t i = 0; i < NUM_WORKERS; i++) {
            workers[i] = new Worker(&r, this, i, chunk_size);
            workers[i]->start();
        }

        for (size_t i = 0; i < NUM_WORKERS; i++) {
            workers[i]->join();
            r.join_delete(workers[i]->rower);
            delete workers[i];
        }

        delete[] workers;
    }
};
