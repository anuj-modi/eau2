#include <gtest/gtest.h>
#include "dataframe.h"

#define GT_TRUE(a) ASSERT_EQ((a), true)
#define GT_FALSE(a) ASSERT_EQ((a), false)
#define GT_EQUALS(a, b) ASSERT_EQ(a, b)
#define ASSERT_EXIT_ZERO(a) ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*")

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
void test() {
    String* name = new String("Name");
    Schema* sp = new Schema();
    Schema s = *sp;
    s.add_column('B', nullptr);
    s.add_column('S', name);
    s.add_row(nullptr);
    s.add_row(name);
    GT_EQUALS(s.col_idx(nullptr), -1);
    GT_EQUALS(s.row_idx(nullptr), -1);

    delete name;
    delete sp;

    exit(0);
}

TEST(a4, t1) {
    ASSERT_EXIT_ZERO(test);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
