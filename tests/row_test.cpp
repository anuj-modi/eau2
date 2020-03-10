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