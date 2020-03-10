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

// fielder class for the map test
class MaxFielder2 : public Fielder {
   public:
    int max1;
    String* max2;
    float max3;
    bool max4;

    MaxFielder2() : Fielder() {
        max1 = 0;
        max2 = nullptr;
        max3 = 0.0;
        max4 = false;
    }

    virtual ~MaxFielder2() {}

    void start(size_t r) {
        max1 = 0;
        max2 = nullptr;
        max3 = 0.0;
        max4 = false;
    }

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b) {
        max4 = b;
    }
    void accept(float f) {
        max3 = f;
    }

    void accept(int i) {
        max1 = i;
    }

    void accept(String* s) {
        max2 = s;
    }

    /** Called when all fields have been seen. */
    virtual void done() {}
};

// rower test for the map test
class MaxRower2 : public Rower {
   public:
    int max1;
    String* max2;
    float max3;
    bool max4;

    MaxRower2() : Rower() {
        max1 = INT_MIN;
        max2 = nullptr;
        max3 = -FLT_MAX;
        max4 = false;
    }

    virtual ~MaxRower2() {}

    bool accept(Row& r) {
        MaxFielder2 mf = MaxFielder2();
        mf.start(r.get_idx());

        for (size_t i = 0; i < r.width(); i++) {
            r.visit(i, mf);
        }
        mf.done();

        if (max1 < mf.max1) {
            max1 = mf.max1;
        }

        if (max2 == nullptr || strcmp(max2->c_str(), mf.max2->c_str()) < 0) {
            max2 = mf.max2;
        }

        if (max3 < mf.max3) {
            max3 = mf.max3;
        }

        if (mf.max4) {
            max4 = true;
        }

        return true;
    }
};

// fielder class for filter test
class FilterGreater10Fielder2 : public Fielder {
   public:
    bool result_;
    FilterGreater10Fielder2() : Fielder() {
        result_ = false;
    }
    virtual ~FilterGreater10Fielder2() {}

    /** Called for fields of the argument's type with the value of the field. */
    void accept(int i) {
        if (i > 10) {
            result_ = true;
        }
    }
};

// rower class for filter test
class FilterGreater102 : public Rower {
   public:
    FilterGreater102() : Rower() {}
    ~FilterGreater102() {}

    bool accept(Row& r) {
        FilterGreater10Fielder2 filter_greater = FilterGreater10Fielder2();
        r.visit(0, filter_greater);
        return filter_greater.result_;
    }
};

/** Tests that adding a column to the dataframe adds a copy of it,
 * since it was mentioned on Piazza that it should be a copy
 */
TEST_CASE("Add column to dataframe is copy", "[column][dataframe]") {
    Schema s("S");
    DataFrame df(s);
    Row r = Row(df.get_schema());
    String* str = new String("Test");
    r.set(0, str);
    df.add_row(r);
    IntColumn* col = new IntColumn();
    col->push_back(4);
    String* col_name = new String("Names");
    df.add_column(col, col_name);
    col->as_int()->set(0, 8);

    REQUIRE(df.get_int(1, 0) == 4);

    delete str;
    delete col_name;
}

/** Tests that adding a column with a length not equal to the
 * df length will not add the column or will add it with padding.
 */
TEST_CASE("uneven column length", "[schema][dataframe]") {
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
        REQUIRE(true);
    } else {
        df.set(1, 0, 11);
        REQUIRE((df.ncols() == 2 && df.get_int(1, 0) == 11));
    }

    delete str;
    delete col;
    delete col_name;
}

 // Tests that constructor for dataframe does not copy over rows.
TEST_CASE("dataframe constructor doesn't copy rows", "[dataframe]") {
    Schema s("I");
    DataFrame df(s);
    Row r = Row(df.get_schema());
    for (int i = 0; i < 10; i++) {
        r.set(0, i);
        df.add_row(r);
    }
    DataFrame df2(df);
    DataFrame df3(df.get_schema());

    REQUIRE(df.nrows() == 10);
    REQUIRE(df2.nrows() == 0);
    REQUIRE(df3.nrows() == 0);
}

// tests that running map works
TEST_CASE("map works", "[dataframe]") {
    Schema s("ISF");
    DataFrame df(s);
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

    df.set(0, 19, 11);
    df.set(1, 94, str2);
    df.set(2, 88, (float)11.0);
    df.set(3, 76, true);
    MaxRower2 mr = MaxRower2();
    df.map(mr);

    REQUIRE(mr.max1 == 11);
    REQUIRE(mr.max2->equals(str2));
    REQUIRE(float_equal(mr.max3, 11.0));
    REQUIRE(mr.max4);

    delete bool_name;
    delete str;
    delete str2;
    delete b;
}

// tests that running filter works
TEST_CASE("filter works", "[dataframe]") {
    Schema s("ISF");
    DataFrame df(s);
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
    df.set(0, 19, 11);
    df.set(1, 94, str2);
    df.set(2, 88, (float)11.0);
    df.set(3, 76, true);
    FilterGreater102 greater_10 = FilterGreater102();
    DataFrame* new_df = df.filter(greater_10);

    REQUIRE(df.ncols() == 4);
    REQUIRE(df.nrows() == 100);
    REQUIRE(new_df->ncols() == 4);
    REQUIRE(new_df->nrows() == 1);

    delete bool_name;
    delete str;
    delete str2;
    delete b;
    delete new_df;
}

// test for dataframe
TEST_CASE("test_dataframe", "[dataframe]") {
    Schema s("ISF");
    DataFrame df(s);
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
    REQUIRE(df.get_col(*bool_name) == 3);

    df.set(0, 19, 11);
    df.set(1, 94, str2);
    df.set(2, 88, (float)11.0);
    df.set(3, 76, true);

    // test get_int and set
    REQUIRE(df.get_int(0, 19) == 11);

    // test get_bool and set
    REQUIRE(df.get_bool(3, 76));

    // test get_float and set
    REQUIRE(float_equal(df.get_float(2, 88), 11.0));

    // test get_string and set
    REQUIRE(df.get_string(1, 94)->equals(str2));

    MaxRower mr = MaxRower();
    df.map(mr);

    // test map
    REQUIRE(mr.maxInt == 11);
    REQUIRE(mr.maxStr->equals(str2));
    REQUIRE(float_equal(mr.maxFloat, 11.0));
    REQUIRE(mr.maxBool);

    FilterGreater10 greater_10 = FilterGreater10();
    DataFrame* new_df = static_cast<DataFrame*>(df.filter(greater_10));

    // test filter
    REQUIRE(new_df->ncols() == 4);
    REQUIRE(new_df->nrows() == 1);

    delete bool_name;
    delete str;
    delete str2;
    delete b;
    delete new_df;
}
