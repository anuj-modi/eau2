#include "../src/dataframe.h"
#include "catch.hpp"

// Tests that schema copy constructor actually makes a copy.
TEST_CASE("schema constructor copies", "[schema]") {
    Schema s("I");
    Schema s2(s);
    s.add_column('S');

    REQUIRE(s.width() == 2);
    REQUIRE(s2.width() == 1);
}

// test adding a column to a schema
TEST_CASE("add column to schema", "[schema]") {
    Schema s("ISD");
    s.add_column('B');

    REQUIRE(s.width() == 4);
    REQUIRE(s.length() == 0);
    REQUIRE(s.col_type(3) == 'B');
}

// test adding a row to a schema
TEST_CASE("add row to schema", "[schema]") {
    Schema s("ISD");
    s.add_row();

    REQUIRE(s.width() == 3);
    REQUIRE(s.length() == 1);
}

// test adding rows to a schema
TEST_CASE("add rows to schema", "[schema]") {
    Schema s("ISD");
    s.add_rows(10);

    REQUIRE(s.width() == 3);
    REQUIRE(s.length() == 10);
}