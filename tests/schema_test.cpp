#include "../src/dataframe.h"
#include "catch.hpp"

/** Tests that methods return -1 if a nullptr name is given for
 * a row or column to look for in schema, since not put as
 * undefined in spec.
 */
TEST_CASE("Basic schema test", "[schema]") {
    String* name = new String("Name");
    Schema* sp = new Schema();
    Schema s = *sp;
    s.add_column('B', nullptr);
    s.add_column('S', name);
    s.add_row(nullptr);
    s.add_row(name);
    REQUIRE(s.col_idx(nullptr) == -1);
    REQUIRE(s.row_idx(nullptr) == -1);

    delete name;
    delete sp;
}

// Tests that schema copy constructor actually makes a copy.
TEST_CASE("schema constructor copies", "[schema]") {
    Schema s("I");
    Schema s2(s);
    String* str = new String("Name");
    s.add_column('I', str);

    REQUIRE(s.width() == 2);
    REQUIRE(s2.width() == 1);

    delete str;
}

// tests for schema
TEST_CASE("test_schema", "[schema]") {
    Schema s("ISF");
    String* col_name = new String("Named Col");
    String* row_name = new String("Named Row");
    s.add_column('B', col_name);
    s.add_row(row_name);
    s.add_row(nullptr);

    // test getting name of a row (nullptr)
    REQUIRE(s.row_name(1) == nullptr);

    // test getting name of a row
    REQUIRE(s.row_name(0)->equals(row_name));

    // test getting index of row (nullptr)
    REQUIRE(s.row_idx(nullptr) == -1);

    // test getting index of row (not in)
    REQUIRE(s.row_idx("not in") == -1);

    // test getting index of row (in)
    REQUIRE(s.row_idx("Named Row") == 0);

    // test getting name of col (nullptr)
    REQUIRE(s.col_name(0) == nullptr);

    // test getting name of col
    REQUIRE(s.col_name(3) == col_name);

    // test getting index of col (nullptr)
    REQUIRE(s.col_idx(nullptr) == -1);

    // test getting index of col (not in)
    REQUIRE(s.col_idx("not in") == -1);

    // test getting index of col (in)
    REQUIRE(s.col_idx("Named Col") == 3);

    // test width of schema
    REQUIRE(s.width() == 4);

    // test length of schema
    REQUIRE(s.length() == 2);

    delete col_name;
    delete row_name;
}
