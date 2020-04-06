#include "store/kvstore.h"

#include "catch.hpp"

// test put and get methods
TEST_CASE("put and get a value in kvstore", "[kvstore]") {
    String s("sdkfak");
    Key k_1 = Key("one");
    Value* v = new Value(s.c_str(), s.size());
    KVStore kv;
    kv.put(k_1, v);

    Value* result = kv.get(k_1);
    REQUIRE(result->equals(v));
    
    delete result;
}

// test put and get methods to remote nodes
TEST_CASE("put and get a value in kvstore remote node", "[kvstore]") {
    String s("sdkfak");
    Key one = Key("one", 1);
    Value* v = new Value(s.c_str(), s.size());
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

    kv0.put(one, v);

    Value* result = kv0.get(one);
    REQUIRE(result->equals(v));

    net0.stop();
    net1.stop();
    net0.join();
    net1.join();

    delete result;
}

// test kvstore waitAndGet
TEST_CASE("waitAndGet a value in kvstore remote node", "[kvstore]") {
    String s("sdkfak");
    Key one = Key("one", 1);
    Value* v = new Value(s.c_str(), s.size());
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

    kv1.put(one, v);
    Value* result = kv0.waitAndGet(one);

    REQUIRE(result->equals(v));

    net0.stop();
    net1.stop();
    net0.join();
    net1.join();

    delete result;
}

// test kvstore this_node
TEST_CASE("check node number in kvstore", "[kvstore]") {
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

    REQUIRE(kv0.this_node() == 0);
    REQUIRE(kv1.this_node() == 1);

    net0.stop();
    net1.stop();
    net0.join();
    net1.join();
}

// test kvstore num_nodes
TEST_CASE("check number of nodes in kvstore", "[kvstore]") {
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

    REQUIRE(kv0.num_nodes() == 2);
    REQUIRE(kv1.num_nodes() == 2);

    net0.stop();
    net1.stop();
    net0.join();
    net1.join();
}