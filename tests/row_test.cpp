#include "catch.hpp"
#include "dataframe/row.h"

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

    delete str;
}

