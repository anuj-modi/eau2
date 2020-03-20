#include "catch.hpp"
#include "dataframe/dataframe.h"
#include "util/serial.h"

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

TEST_CASE("serialize and deserialize schema no rows", "[schema][serialize][deserialize]") {
    Schema s("ISDB");
    REQUIRE(s.width() == 4);
    REQUIRE(s.length() == 0);

    Serializer sr;
    s.serialize(&sr);

    Deserializer d(sr.get_bytes(), sr.size());
    Schema s2(&d);
    REQUIRE(d.bytes_remaining_ == 0);
    REQUIRE(s2.width() == s.width());
    REQUIRE(s2.length() == s.length());

    for (size_t i = 0; i < s.width(); i++) {
        REQUIRE(s.col_type(i) == s2.col_type(i));
    }
}

TEST_CASE("serialize and deserialize schema some rows", "[schema][serialize][deserialize]") {
    Schema s("ISDB");
    s.add_rows(100);
    REQUIRE(s.width() == 4);
    REQUIRE(s.length() == 100);

    Serializer sr;
    s.serialize(&sr);

    Deserializer d(sr.get_bytes(), sr.size());
    Schema s2(&d);
    REQUIRE(d.bytes_remaining_ == 0);
    REQUIRE(s2.width() == s.width());
    REQUIRE(s2.length() == s.length());

    for (size_t i = 0; i < s.width(); i++) {
        REQUIRE(s.col_type(i) == s2.col_type(i));
    }
}
