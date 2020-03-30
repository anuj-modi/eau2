#include "util/array.h"
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

// tests that all push_backs and gets work
TEST_CASE("push_back and get for all types", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");

    IntArray *l2 = new IntArray(5);
    l2->push_back(3);
    l2->push_back(4);
    REQUIRE((l2->get_int(0) == 3 && l2->get_int(1) == 4 && l2->size() == 2));

    float u = -3.3;
    float v = 4.4;
    DoubleArray *l3 = new DoubleArray(5);
    l3->push_back(u);
    l3->push_back(v);
    REQUIRE((float_equal(l3->get_double(0), -3.3) && float_equal(l3->get_double(1), 4.4) &&
             l3->size() == 2));

    BoolArray *l4 = new BoolArray(5);
    l4->push_back(true);
    l4->push_back(false);
    REQUIRE((l4->get_bool(0) && !l4->get_bool(1) && l4->size() == 2));

    StringArray *l6 = new StringArray(5);
    l6->push_back(s);
    l6->push_back(t);
    REQUIRE((l6->get_string(0)->equals(s) && l6->get_string(1)->equals(t) && l6->size() == 2));

    delete l2;
    delete l3;
    delete l4;
    delete l6;
}
