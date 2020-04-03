#include "store/kvstore.h"
#include "catch.hpp"

// test in method
TEST_CASE("key in kvstore", "[kvstore]") {
    Key k_1 = Key("one");
    Key k_2 = Key("two");
    String s("sdkfak");
    Value* v = new Value(s.c_str(), s.size());
    KVStore kv;
    kv.put(k_1, v);

    REQUIRE(kv.in(k_1));
    REQUIRE_FALSE(kv.in(k_2));
}

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

// TODO test kvstore waitAndGet

// TODO test kvstore this_node

// TODO test kvstore num_nodes