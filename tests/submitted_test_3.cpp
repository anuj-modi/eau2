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

/** Tests that adding a column with a length not equal to the
 * df length will not add the column or will add it with padding.
 */
void test() {
    bool first_opt = false;
    bool second_opt = false;
    Schema s("S");
    DataFrame df(s);
    Row r = Row(df.get_schema());
    String* str = new String("Test");
    r.set(0, str);
    df.add_row(r);
    IntColumn* col = new IntColumn();
    String* col_name = new String("Names");
    df.add_column(col, col_name);

    if (df.ncols() == 1) {
        GT_TRUE(true);
    } else {
        df.set(1, 0, 11);
        GT_TRUE(df.ncols() == 2 && df.get_int(1, 0) == 11);
    }

    delete str;
    delete col;
    delete col_name;

    exit(0);
}


TEST(a4, t1) {
    ASSERT_EXIT_ZERO(test);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}