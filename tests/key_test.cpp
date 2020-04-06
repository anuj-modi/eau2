#include "store/key.h"

#include "catch.hpp"
#include "util/serial.h"

// test equal method
TEST_CASE("equal for key", "[key]") {
    Key k_1("first");
    Key* same = new Key("first", 1);
    Key* different = new Key("second", 3);

    REQUIRE(k_1.equals(same));
    REQUIRE_FALSE(k_1.equals(different));

    delete same;
    delete different;
}

// test hash method
TEST_CASE("hash a key", "[key]") {
    Key k_1("first");
    Key* same = new Key("first", 0);
    Key* different = new Key("second", 3);

    REQUIRE(k_1.hash() == same->hash());
    REQUIRE_FALSE(k_1.hash() == different->hash());

    delete same;
    delete different;
}

// test clone method
TEST_CASE("clone a key", "[key]") {
    Key k("first");
    Key* k_clone = k.clone();
    REQUIRE(k.equals(k_clone));

    delete k_clone;
}

// test set_node
TEST_CASE("set node key's node", "[key]") {
    Key k("first");
    REQUIRE(k.node_ == 0);
    k.set_node(5);
    REQUIRE(k.node_ == 5);
}

TEST_CASE("serialize deserialize key", "[key][serialize][deserialize]") {
    Key k("asdf");
    Serializer s;
    k.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());

    Key k2(&d);

    REQUIRE(k2.k_ == k.k_);
    REQUIRE(k2.node_ == k.node_);
}
