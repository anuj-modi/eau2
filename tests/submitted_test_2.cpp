// #include <gtest/gtest.h>
// #include "dataframe.h"

// #define GT_TRUE(a) ASSERT_EQ((a), true)
// #define GT_FALSE(a) ASSERT_EQ((a), false)
// #define GT_EQUALS(a, b) ASSERT_EQ(a, b)
// #define ASSERT_EXIT_ZERO(a) ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*")

// /**
//  * Determine if these two floats are equal with respect to eps.
//  * @param f1 the first float to compare.
//  * @param f2 the second float to compare.
//  */
// bool float_equal(float f1, float f2) {
//     float eps = 0.0000001;
//     if (f1 > f2) {
//         return f1 - f2 < eps;
//     } else {
//         return f2 - f1 < eps;
//     }
// }

// /** Tests that adding a column to the dataframe adds a copy of it,
//  * since it was mentioned on Piazza that it should be a copy
//  */
// void test() {
//     Schema s("S");
//     DataFrame df(s);
//     Row r = Row(df.get_schema());
//     String* str = new String("Test");
//     r.set(0, str);
//     df.add_row(r);
//     IntColumn* col = new IntColumn();
//     col->push_back(4);
//     String* col_name = new String("Names");
//     df.add_column(col, col_name);
//     col->as_int()->set(0, 8);

//     GT_EQUALS(df.get_int(1, 0), 4);

//     delete str;
//     delete col_name;

//     exit(0);
// }

// TEST(a4, t1) {
//     ASSERT_EXIT_ZERO(test);
// }

// int main(int argc, char** argv) {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
