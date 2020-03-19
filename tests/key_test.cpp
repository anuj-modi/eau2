#include "catch.hpp"
#include "store/key.h"

// test equal method
TEST_CASE("equal for key", "[key]") {
    Key k_1("first");
    Key* same = new Key("first", 0);
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