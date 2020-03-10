#include "catch.hpp"
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

// tests for row
void test_row() {
    Schema s("ISFB");
    Row r(s);
    String* str = new String("Test");
    r.set(0, 1);
    r.set(1, str);
    r.set(2, 1.3f);
    r.set(3, true);

    // tests if int value is set and retreived properly
    GT_EQUALS(r.get_int(0), 1);

    // tests if string value is set and retreived properly
    GT_TRUE(r.get_string(1)->equals(str));

    // tests if float value is set and retreived properly
    GT_TRUE(float_equal(r.get_float(2), 1.3));

    // tests if bool is set and retreived properly
    GT_TRUE(r.get_bool(3));

    // tests if width of row is correct
    GT_EQUALS(r.width(), 4);

    delete str;

    exit(0);
}

// tests for schema
void test_schema() {
    Schema s("ISF");
    String* col_name = new String("Named Col");
    String* row_name = new String("Named Row");
    s.add_column('B', col_name);
    s.add_row(row_name);
    s.add_row(nullptr);

    // test getting name of a row (nullptr)
    GT_EQUALS(s.row_name(1), nullptr);

    // test getting name of a row
    GT_TRUE(s.row_name(0)->equals(row_name));

    // test getting index of row (nullptr)
    GT_EQUALS(s.row_idx(nullptr), -1);

    // test getting index of row (not in)
    GT_EQUALS(s.row_idx("not in"), -1);

    // test getting index of row (in)
    GT_EQUALS(s.row_idx("Named Row"), 0);

    // test getting name of col (nullptr)
    GT_EQUALS(s.col_name(0), nullptr);

    // test getting name of col
    GT_EQUALS(s.col_name(3), col_name);

    // test getting index of col (nullptr)
    GT_EQUALS(s.col_idx(nullptr), -1);

    // test getting index of col (not in)
    GT_EQUALS(s.col_idx("not in"), -1);

    // test getting index of col (in)
    GT_EQUALS(s.col_idx("Named Col"), 3);

    // test width of schema
    GT_EQUALS(s.width(), 4);

    // test length of schema
    GT_EQUALS(s.length(), 2);

    delete col_name;
    delete row_name;

    exit(0);
}

// TODO move in colomn tests

// test for dataframe
void test_dataframe() {
    Schema s("ISF");
    DataFrame df(s);
    String* nullstr = nullptr;
    String* bool_name = new String("Bool Col");
    BoolColumn* b = new BoolColumn();
    df.add_column(b, bool_name);
    Row r = Row(df.get_schema());
    String* str = new String("Test");
    String* str2 = new String("ZZZZZZZZ");
    for (int i = 0; i < 100; i++) {
        r.set(0, 8);
        r.set(1, str);
        r.set(2, 8.0f);
        r.set(3, false);
        df.add_row(r);
    }

    // test get_col
    GT_EQUALS(df.get_col(*bool_name), 3);

    df.set(0, 19, 11);
    df.set(1, 94, str2);
    df.set(2, 88, (float)11.0);
    df.set(3, 76, true);

    // test get_int and set
    GT_EQUALS(df.get_int(0, 19), 11);

    // test get_bool and set
    GT_TRUE(df.get_bool(3, 76));

    // test get_float and set
    GT_TRUE(float_equal(df.get_float(2, 88), 11.0));

    // test get_string and set
    GT_TRUE(df.get_string(1, 94)->equals(str2));

    MaxRower mr = MaxRower();
    df.map(mr);

    // test map
    GT_EQUALS(mr.max1, 11);
    GT_TRUE(mr.max2->equals(str2));
    GT_TRUE(float_equal(mr.max3, 11.0));
    GT_TRUE(mr.max4);

    FilterGreater10 greater_10 = FilterGreater10();
    DataFrame* new_df = df.filter(greater_10);

    // test filter
    GT_EQUALS(new_df->ncols(), 4);
    GT_EQUALS(new_df->nrows(), 1);

    delete bool_name;
    delete str;
    delete str2;
    delete b;
    delete new_df;

    exit(0);
}

TEST(a4, schema) {
    ASSERT_EXIT_ZERO(test_schema);
}

TEST(a4, row) {
    ASSERT_EXIT_ZERO(test_row);
}

TEST(a4, dataframe) {
    ASSERT_EXIT_ZERO(test_dataframe);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
