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