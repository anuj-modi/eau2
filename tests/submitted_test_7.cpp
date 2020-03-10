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

// class FilterGreater10Fielder2 : public Fielder {
//    public:
//     bool result_;
//     FilterGreater10Fielder2() : Fielder() {
//         result_ = false;
//     }
//     virtual ~FilterGreater10Fielder2() {}

//     /** Called for fields of the argument's type with the value of the field. */
//     void accept(int i) {
//         if (i > 10) {
//             result_ = true;
//         }
//     }
// };

// class FilterGreater102 : public Rower {
//    public:
//     FilterGreater102() : Rower() {}
//     ~FilterGreater102() {}

//     bool accept(Row& r) {
//         FilterGreater10Fielder2 filter_greater = FilterGreater10Fielder2();
//         r.visit(0, filter_greater);
//         return filter_greater.result_;
//     }
// };

// /** Tests that methods return -1 if a nullptr name is given for
//  * a row or column to look for in schema, since not put as
//  * undefined in spec.
//  */
// void test() {
//     Schema s("ISF");
//     DataFrame df(s);
//     String* bool_name = new String("Bool Col");
//     BoolColumn* b = new BoolColumn();
//     df.add_column(b, bool_name);
//     Row r = Row(df.get_schema());
//     String* str = new String("Test");
//     String* str2 = new String("ZZZZZZZZ");
//     for (int i = 0; i < 100; i++) {
//         r.set(0, 8);
//         r.set(1, str);
//         r.set(2, 8.0f);
//         r.set(3, false);
//         df.add_row(r);
//     }
//     df.set(0, 19, 11);
//     df.set(1, 94, str2);
//     df.set(2, 88, (float)11.0);
//     df.set(3, 76, true);
//     FilterGreater102 greater_10 = FilterGreater102();
//     DataFrame* new_df = df.filter(greater_10);

//     // test filter
//     GT_EQUALS(new_df->ncols(), 4);
//     GT_EQUALS(new_df->nrows(), 1);

//     delete bool_name;
//     delete str;
//     delete str2;
//     delete b;
//     delete new_df;

//     exit(0);
// }

// TEST(a4, t1) {
//     ASSERT_EXIT_ZERO(test);
// }

// int main(int argc, char** argv) {
//     testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
