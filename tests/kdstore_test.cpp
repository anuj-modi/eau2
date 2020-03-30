#include "store/kdstore.h"
#include <vector>
#include "catch.hpp"

// test in method
TEST_CASE("key in kdstore", "[kdstore]") {
    Key k_1 = Key("one");
    Key k_2 = Key("two");
    String blah("Blah");
    KVStore kv;
    StringColumn* sc = new StringColumn(&kv);
    sc->push_back(&blah);

    std::vector<Column*> cs;
    cs.push_back(sc);
    DataFrame df(cs, &kv);

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
    IntColumn* ic = new IntColumn(&kv);
    ic->push_back(5);

    std::vector<Column*> cs;
    cs.push_back(ic);
    DataFrame df(cs, &kv);
    
    KDStore kd(&kv);
    kd.put(k_1, &df);
    DataFrame* copy_df = kd.get(k_1);

    REQUIRE(copy_df->get_int(0, 0) == 5);
    REQUIRE(copy_df->ncols() == 1);
    REQUIRE(copy_df->nrows() == 1);

    delete copy_df;
}

// TODO test kdstore waitAndGet
