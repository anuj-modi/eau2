#include "../src/dataframe.h"
#include "catch.hpp"

// tests for row
TEST_CASE("Basic test cases for Row", "[row]") {
    Schema s("ISFB");
    Row r(s);
    String* str = new String("Test");
    r.set(0, 1);
    r.set(1, str);
    r.set(2, 1.3f);
    r.set(3, true);

    // tests if int value is set and retreived properly
    REQUIRE(r.get_int(0) == 1);

    // tests if string value is set and retreived properly
    REQUIRE(r.get_string(1)->equals(str));

    // tests if float value is set and retreived properly
    REQUIRE(float_equal(r.get_float(2), 1.3));

    // tests if bool is set and retreived properly
    REQUIRE(r.get_bool(3));

    // tests if width of row is correct
    REQUIRE(r.width() == 4);

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

// test for dataframe
TEST_CASE("test_dataframe", "[dataframe]") {
    Schema s("ISF");
    DataFrame df(s);
    String* bool_name = new String("Bool Col");
    BoolColumn* b = new BoolColumn();
    df.add_column(b, bool_name);
    Row r = Row(df.get_schema());
    String* str = new String("Test");
    String* str2 = new String("ZZZZZZZZ");
    for (int i = 0; i < 100; i++) {
        r.set(0, 8);
        r.set(1, str);
        r.set(2, 8.0f);
        r.set(3, false);
        df.add_row(r);
    }

    // test get_col
    REQUIRE(df.get_col(*bool_name) == 3);

    df.set(0, 19, 11);
    df.set(1, 94, str2);
    df.set(2, 88, (float)11.0);
    df.set(3, 76, true);

    // test get_int and set
    REQUIRE(df.get_int(0, 19) == 11);

    // test get_bool and set
    REQUIRE(df.get_bool(3, 76));

    // test get_float and set
    REQUIRE(float_equal(df.get_float(2, 88), 11.0));

    // test get_string and set
    REQUIRE(df.get_string(1, 94)->equals(str2));

    MaxRower mr = MaxRower();
    df.map(mr);

    // test map
    REQUIRE(mr.maxInt == 11);
    REQUIRE(mr.maxStr->equals(str2));
    REQUIRE(float_equal(mr.maxFloat, 11.0));
    REQUIRE(mr.maxBool);

    FilterGreater10 greater_10 = FilterGreater10();
    DataFrame* new_df = static_cast<DataFrame*>(df.filter(greater_10));

    // test filter
    REQUIRE(new_df->ncols() == 4);
    REQUIRE(new_df->nrows() == 1);

    delete bool_name;
    delete str;
    delete str2;
    delete b;
    delete new_df;
}
