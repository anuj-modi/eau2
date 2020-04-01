#include "catch.hpp"
#include "dataframe/row.h"
#include "dataframe/column.h"

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

// tests set and get
TEST_CASE("set and get from a row", "[row]") {
    Schema s("ISDB");
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

    // tests if double value is set and retreived properly
    REQUIRE(double_equal(r.get_double(2), 1.3));

    // tests if bool is set and retreived properly
    REQUIRE(r.get_bool(3));

    // tests if width of row is correct
    REQUIRE(r.width() == 4);
}

// test add_to_columns method
TEST_CASE("add from row to columns", "[row]") {
    Schema s("ISDB");
    Row r(s);
    String* str = new String("Test");
    r.set(0, -1);
    r.set(1, str);
    r.set(2, -1.3f);
    r.set(3, true);
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    BoolColumn* bc = new BoolColumn(&kv);
    StringColumn* sc = new StringColumn(&kv);
    DoubleColumn* dc = new DoubleColumn(&kv);
    String* str_2 = new String("Test2");
    for (int i = 0; i < 100; i++) {
        ic->push_back(i);
        sc->push_back(str_2);
        dc->push_back(i);
        bc->push_back(false);
    }
    std::vector<Column*> cs = std::vector<Column*>();
    cs.push_back(ic);
    cs.push_back(sc);
    cs.push_back(dc);
    cs.push_back(bc);
    r.add_to_columns(cs);

    ic->finalize();
    bc->finalize();
    sc->finalize();
    dc->finalize();

    REQUIRE(ic->get(100) == -1);
    String* result = sc->get(100);
    REQUIRE(result->equals(str));
    REQUIRE(double_equal(dc->get(100), -1.3f));
    REQUIRE(bc->get(100));

    delete str_2;
    delete result;
    delete ic;
    delete bc;
    delete dc;
    delete sc;
}

