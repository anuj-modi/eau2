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

// Tests if constructor for dataframe does not copy over rows.
void test() {
    Schema s("I");
    DataFrame df(s);
    Row r = Row(df.get_schema());
    for (int i = 0; i < 10; i++) {
        r.set(0, i);
        df.add_row(r);
    }
    DataFrame df2(df);
    DataFrame df3(df.get_schema());
    GT_EQUALS(df.nrows(), 10);
    GT_EQUALS(df2.nrows(), 0);
    GT_EQUALS(df3.nrows(), 0);

    exit(0);
}


TEST(a4, t1) {
    ASSERT_EXIT_ZERO(test);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}