#include "../src/dataframe.h"
#include "catch.hpp"

/**
 * Determine if these two floats are equal with respect to eps.
 * @param f1 the first float to compare.
 * @param f2 the second float to compare.
 */
bool float_equal(float f1, float f2) {
    float eps = 0.0000001;
    if (f1 > f2) {
        return f1 - f2 < eps;
    } else {
        return f2 - f1 < eps;
    }
}

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
