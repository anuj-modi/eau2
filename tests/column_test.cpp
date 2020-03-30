#include "catch.hpp"
#include "dataframe/dataframe.h"

/**
 * Determine if these two doubles are equal with respect to eps.
 * @param f1 the first double to compare.
 * @param f2 the second double to compare.
 */
static bool double_equal(double f1, double f2) {
    double eps = 0.0000001;
    if (f1 > f2) {
        return f1 - f2 < eps;
    } else {
        return f2 - f1 < eps;
    }
}

// tests for an Int Column
TEST_CASE("test int column", "[column]") {
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    int size = 1000;
    for (int i = 0; i < size; i++) {
        ic->push_back(i);
    }
    ic->finalize();

    for (int i = 0; i < size; i++) {
        REQUIRE(ic->get(i) == i);
    }

    REQUIRE(ic->get_type() == 'I');
    REQUIRE(ic->size() == size);
    REQUIRE(ic->as_int() == ic);
    REQUIRE(ic->as_bool() == nullptr);
    REQUIRE(ic->as_double() == nullptr);
    REQUIRE(ic->as_string() == nullptr);

    delete ic;
}

// tests for a double Column
TEST_CASE("test double column", "[column]") {
    KVStore kv;
    DoubleColumn* fc = new DoubleColumn(&kv);
    int size = 1000;
    for (int i = 0; i < size; i++) {
        fc->push_back(i * 1.5f);
    }
    fc->finalize();

    REQUIRE(fc->get_type() == 'D');

    for (int i = 0; i < size; i++) {
        REQUIRE(double_equal(fc->get(i), i * 1.5f));
    }

    REQUIRE(fc->size() == size);
    REQUIRE(fc->as_int() == nullptr);
    REQUIRE(fc->as_double() == fc);
    REQUIRE(fc->as_bool() == nullptr);
    REQUIRE(fc->as_string() == nullptr);

    delete fc;
}

// tests for an bool Column
TEST_CASE("test bool column", "[column]") {
    KVStore kv;
    BoolColumn* bc = new BoolColumn(&kv);
    int size = 10000;
    for (int i = 0; i < size; i++) {
        bool val = i % 2 ? false : true;
        bc->push_back(val);
    }
    bc->finalize();

    REQUIRE(bc->get_type() == 'B');

    for (int i = 0; i < size; i++) {
        bool val = i % 2 ? false : true;
        REQUIRE(bc->get(i) == val);
    }

    REQUIRE(bc->size() == size);
    REQUIRE(bc->as_int() == nullptr);
    REQUIRE(bc->as_bool() == bc);
    REQUIRE(bc->as_double() == nullptr);
    REQUIRE(bc->as_string() == nullptr);

    delete bc;
}

TEST_CASE("test string column", "[column]") {
    KVStore kv;
    StringColumn* sc = new StringColumn(&kv);
    String* a = new String("a");
    String* b = new String("b");
    String* c = new String("c");
    String* d = new String("d");
    String* e = new String("e");

    sc->push_back(a);
    sc->push_back(b);
    sc->push_back(c);
    sc->finalize();

    REQUIRE(sc->size() == 3);
    REQUIRE(sc->get_type() == 'S');
    String* s = sc->get(0);
    REQUIRE(a->equals(s));

    REQUIRE(sc->as_int() == nullptr);
    REQUIRE(sc->as_bool() == nullptr);
    REQUIRE(sc->as_double() == nullptr);
    REQUIRE(sc->as_string() == sc);

    delete a;
    delete b;
    delete c;
    delete d;
    delete e;
    delete s;
    delete sc;
}
