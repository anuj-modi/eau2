#pragma once
#include "dataframe_base.h"
#include "rower.h"
#include "thread.h"

class Worker : public Thread {
   public:
    Rower* rower;
    DataFrameBase* df_;
    size_t worker_num_;
    size_t chunk_size_;
    size_t leftovers_;

    Worker() : Thread() {}

    Worker(Rower* r, DataFrameBase* df, size_t worker_num, size_t chunk_size) : Worker(r, df, worker_num, chunk_size, 0) {}

    Worker(Rower* r, DataFrameBase* df, size_t worker_num, size_t chunk_size, size_t leftovers) : Worker() {
        rower = r->clone();
        df_ = df;
        worker_num_ = worker_num;
        chunk_size_ = chunk_size;
        leftovers_ = leftovers;
    }

    virtual ~Worker() {}

    virtual void run() {
        size_t start_row = worker_num_ * chunk_size_;
        size_t end_row = start_row + chunk_size_ + leftovers_;
        for (size_t i = start_row; i < end_row; i++) {
            Row curr_row = Row(*(df_->df_schema_));
            df_->fill_row(i, curr_row);
            rower->accept(curr_row);
        }
    }
};
