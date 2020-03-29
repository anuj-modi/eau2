#include "store/kdstore.h"
#include "catch.hpp"

// test in method
TEST_CASE("key in kdstore", "[kdstore]") {
    Key k_1 = Key("one");
    Key k_2 = Key("two");
    Schema s("I");
    KVStore kv;
    DataFrame df(s, &kv);
    Row r(s);
    r.set(0, 5);
    df.add_row(r);
    KDStore kd(&kv);
    kd.put(k_1, &df);

    REQUIRE(kd.in(k_1));
    REQUIRE_FALSE(kd.in(k_2));
}

// test get and put methods
TEST_CASE("put and get a value in kdstore", "[kdstore]") {
    Key k_1 = Key("one");
    Schema s("I");
    KVStore kv;
    DataFrame df(s, &kv);
    Row r(s);
    r.set(0, 5);
    df.add_row(r);
    KDStore kd(&kv);
    kd.put(k_1, &df);
    DataFrame* copy_df = kd.get(k_1);

    REQUIRE(copy_df->get_int(0, 0) == 5);
    REQUIRE(copy_df->ncols() == 1);
    REQUIRE(copy_df->nrows() == 1);

    delete copy_df;
}

// TODO test kdstore waitAndGet
