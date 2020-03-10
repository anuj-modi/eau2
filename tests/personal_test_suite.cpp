#include "../src/dataframe.h"
#include "catch.hpp"

/**
 * Determine if these two floats are equal with respect to eps.
 * @param f1 the first float to compare.
 * @param f2 the second float to compare.
 */
static bool float_equal(float f1, float f2) {
    float eps = 0.0000001;
    if (f1 > f2) {
        return f1 - f2 < eps;
    } else {
        return f2 - f1 < eps;
    }
}

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

// tests for an Int Column
TEST_CASE("test_int_column", "[column]") {
    IntColumn* ic = new IntColumn();
    int size = 10000;
    for (int i = 0; i < size; i++) {
        ic->push_back(i);
    }

    REQUIRE(ic->get_type() == 'I');

    for (int i = 0; i < size; i++) {
        REQUIRE(ic->get(i) == i);
    }

    REQUIRE(ic->size() == size);
    ic->set(size - 1, 9001);
    for (int i = 0; i < size - 1; i++) {
        REQUIRE(ic->get(i) == i);
    }

    REQUIRE(ic->size() == size);
    REQUIRE(ic->get(size - 1) == 9001);

    REQUIRE(ic->as_int() == ic);
    REQUIRE(ic->as_bool() == nullptr);
    REQUIRE(ic->as_float() == nullptr);
    REQUIRE(ic->as_string() == nullptr);

    delete ic;
}

// tests for a Float Column
TEST_CASE("test_float_column", "[column]") {
    FloatColumn* fc = new FloatColumn();
    int size = 1000;
    for (int i = 0; i < size; i++) {
        fc->push_back(i * 1.5f);
    }

    REQUIRE(fc->get_type() == 'F');

    for (int i = 0; i < size; i++) {
        REQUIRE(float_equal(fc->get(i), i * 1.5f));
    }

    REQUIRE(fc->size() == size);
    fc->set(size - 1, 9001.9001);
    for (int i = 0; i < size - 1; i++) {
        REQUIRE(float_equal(fc->get(i), i * 1.5f));
    }

    REQUIRE(fc->size() == size);
    REQUIRE(float_equal(fc->get(size - 1), 9001.9001));

    REQUIRE(fc->as_int() == nullptr);
    REQUIRE(fc->as_float() == fc);
    REQUIRE(fc->as_bool() == nullptr);
    REQUIRE(fc->as_string() == nullptr);

    delete fc;
}
// tests for an Int Column
TEST_CASE("test_bool_column", "[column]") {
    BoolColumn* bc = new BoolColumn();
    int size = 10000;
    for (int i = 0; i < size; i++) {
        bool val = i % 2 ? false : true;
        bc->push_back(val);
    }

    REQUIRE(bc->get_type() == 'B');

    for (int i = 0; i < size; i++) {
        bool val = i % 2 ? false : true;
        REQUIRE(bc->get(i) == val);
    }

    REQUIRE(bc->size() == size);
    bool old_last = bc->get(size - 1);
    bc->set(size - 1, !old_last);
    for (int i = 0; i < size - 1; i++) {
        if (i % 2 == 0) {
            REQUIRE(bc->get(i));
        } else {
            REQUIRE_FALSE(bc->get(i));
        }
    }

    REQUIRE(bc->size() == size);
    REQUIRE(bc->get(size - 1) == !old_last);

    REQUIRE(bc->as_int() == nullptr);
    REQUIRE(bc->as_bool() == bc);
    REQUIRE(bc->as_float() == nullptr);
    REQUIRE(bc->as_string() == nullptr);

    delete bc;
}

TEST_CASE("test_string_column", "[column]") {
    StringColumn* sc = new StringColumn();
    String* a = new String("a");
    String* b = new String("b");
    String* c = new String("c");
    String* d = new String("d");
    String* e = new String("e");

    sc->push_back(a);
    sc->push_back(b);
    sc->push_back(c);

    REQUIRE(sc->size() == 3);
    REQUIRE(sc->get_type() == 'S');
    REQUIRE(a->equals(sc->get(0)));

    REQUIRE(sc->as_int() == nullptr);
    REQUIRE(sc->as_bool() == nullptr);
    REQUIRE(sc->as_float() == nullptr);
    REQUIRE(sc->as_string() == sc);

    delete a;
    delete b;
    delete c;
    delete d;
    delete e;
    delete sc;
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
