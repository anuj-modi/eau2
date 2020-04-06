#include "store/kdstore.h"

#include <vector>

#include "catch.hpp"

// test get and put methods
TEST_CASE("put and get a value in kdstore", "[kdstore]") {
    Key k_1 = Key("one");
    Schema s("I");
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    ic->push_back(5);
    DataFrame df(ic, &kv);

    KDStore kd(&kv);
    kd.put(k_1, &df);
    DataFrame* copy_df = kd.get(k_1);

    REQUIRE(copy_df->get_int(0, 0) == 5);
    REQUIRE(copy_df->ncols() == 1);
    REQUIRE(copy_df->nrows() == 1);

    delete copy_df;
}

// TODO test kdstore waitAndGet
