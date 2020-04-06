#include "store/kdstore.h"

#include <vector>

#include "catch.hpp"

// test get and put methods
TEST_CASE("put and get a df in kdstore", "[kdstore]") {
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

// test get and put methods remote node
TEST_CASE("put and get a df in kdstore remote node", "[kdstore]") {
    Key one = Key("one", 1);
    Schema s("I");
    Address a0("127.0.0.1", 10000);
    Address a1("127.0.0.1", 10001);
    NetworkIfc net0(&a0, 2);
    KVStore kv0(&net0);
    net0.set_kv(&kv0);
    NetworkIfc net1(&a1, &a0, 1, 2);
    KVStore kv1(&net1);
    net1.set_kv(&kv1);

    net0.start();
    net1.start();

    IntColumn* ic = new IntColumn(&kv0);
    ic->push_back(5);
    DataFrame df(ic, &kv0);

    KDStore kd(&kv0);
    kd.put(one, &df);
    DataFrame* copy_df = kd.get(one);

    REQUIRE(copy_df->get_int(0, 0) == 5);
    REQUIRE(copy_df->ncols() == 1);
    REQUIRE(copy_df->nrows() == 1);

    net0.stop();
    net1.stop();
    net0.join();
    net1.join();

    delete copy_df;
}

// // test kdstore waitAndGet
TEST_CASE("getAndWait a df in kdstore remote node", "[kdstore]") {
    Key one = Key("one", 1);
    Schema s("I");
    Address a0("127.0.0.1", 10000);
    Address a1("127.0.0.1", 10001);
    NetworkIfc net0(&a0, 2);
    KVStore kv0(&net0);
    net0.set_kv(&kv0);
    NetworkIfc net1(&a1, &a0, 1, 2);
    KVStore kv1(&net1);
    net1.set_kv(&kv1);

    net0.start();
    net1.start();

    IntColumn* ic = new IntColumn(&kv0);
    ic->push_back(5);
    DataFrame df(ic, &kv0);

    KDStore kd0(&kv0);
    KDStore kd1(&kv1);

    kd1.put(one, &df);
    DataFrame* copy_df = kd0.waitAndGet(one);

    REQUIRE(copy_df->get_int(0, 0) == 5);
    REQUIRE(copy_df->ncols() == 1);
    REQUIRE(copy_df->nrows() == 1);

    net0.stop();
    net1.stop();
    net0.join();
    net1.join();

    delete copy_df;
}
