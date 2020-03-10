#include "../src/dataframe.h"
#include "catch.hpp"

// Tests that schema copy constructor actually makes a copy.
TEST_CASE("schema constructor copies", "[schema][dataframe]") {
    Schema s("I");
    Schema s2(s);
    String* str = new String("Name");
    s.add_column('I', str);

    REQUIRE(s.width() == 2);
    REQUIRE(s2.width() == 1);

    delete str;
}
