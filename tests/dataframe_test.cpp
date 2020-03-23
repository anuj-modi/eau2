#include "dataframe/dataframe.h"
#include "catch.hpp"

/**
 * Determine if these two doubles are equal with respect to eps.
 * @param f1 the first double to compare.
 * @param f2 the second double to compare.
 */
static bool double_equal(double f1, double f2) {
    double eps = 0.0000001;
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
    double max3;
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
    void accept(double f) {
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
    int maxInt;
    String* maxStr;
    double maxFloat;
    bool maxBool;

    MaxRower2() : Rower() {
        maxInt = INT_MIN;
        maxStr = nullptr;
        maxFloat = -FLT_MAX;
        maxBool = false;
    }

    virtual ~MaxRower2() {}

    bool accept(Row& r) {
        MaxFielder2 mf = MaxFielder2();
        mf.start(r.get_idx());

        for (size_t i = 0; i < r.width(); i++) {
            r.visit(i, mf);
        }
        mf.done();

        if (maxInt < mf.max1) {
            maxInt = mf.max1;
        }

        if (maxStr == nullptr || strcmp(maxStr->c_str(), mf.max2->c_str()) < 0) {
            maxStr = mf.max2;
        }

        if (maxFloat < mf.max3) {
            maxFloat = mf.max3;
        }

        if (mf.max4) {
            maxBool = true;
        }

        return true;
    }

    void join_delete(Rower* r) {
        MaxRower2* mr = dynamic_cast<MaxRower2*>(r);
        if (mr == nullptr) {
            return;
        }

        if (maxInt < mr->maxInt) {
            maxInt = mr->maxInt;
        }

        if (maxStr == nullptr || strcmp(maxStr->c_str(), mr->maxStr->c_str()) < 0) {
            maxStr = mr->maxStr;
        }

        if (maxFloat < mr->maxFloat) {
            maxFloat = mr->maxFloat;
        }

        if (mr->maxBool) {
            maxBool = true;
        }
        delete mr;
    }

    Rower* clone() {
        return new MaxRower2();
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
    df.add_column(col);
    col->as_int()->set(0, 8);

    REQUIRE(df.get_int(1, 0) == 4);

    delete str;
    delete col;
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
    Schema s("ISD");
    DataFrame df(s);
    BoolColumn* b = new BoolColumn();
    df.add_column(b);
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
    df.set(2, 88, (double)11.0);
    df.set(3, 76, true);
    MaxRower2 mr = MaxRower2();
    df.map(mr);

    REQUIRE(mr.maxInt == 11);
    REQUIRE(mr.maxStr->equals(str2));
    REQUIRE(double_equal(mr.maxFloat, 11.0));
    REQUIRE(mr.maxBool);

    delete str;
    delete str2;
    delete b;
}

// tests that running filter works
TEST_CASE("filter works", "[dataframe]") {
    Schema s("ISD");
    DataFrame df(s);
    BoolColumn* b = new BoolColumn();
    df.add_column(b);
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
    df.set(2, 88, (double)11.0);
    df.set(3, 76, true);
    FilterGreater102 greater_10 = FilterGreater102();
    DataFrame* new_df = df.filter(greater_10);

    REQUIRE(df.ncols() == 4);
    REQUIRE(df.nrows() == 100);
    REQUIRE(new_df->ncols() == 4);
    REQUIRE(new_df->nrows() == 1);

    delete str;
    delete str2;
    delete b;
    delete new_df;
}

// test adding column to the dataframe
TEST_CASE("adding column to dataframe", "[dataframe]") {
    Schema s("ISD");
    DataFrame df(s);
    BoolColumn* b = new BoolColumn();
    df.add_column(b);

    REQUIRE(df.ncols() == 4);
    REQUIRE(df.df_schema_->col_type(3) == 'B');

    delete b;
}

// test adding a row to the dataframe
TEST_CASE("adding row to dataframe", "[dataframe]") {
    Schema s("IDB");
    DataFrame df(s);
    Row r = Row(df.get_schema());
    r.set(0, 8);
    r.set(1, 8.0f);
    r.set(2, false);
    df.add_row(r);

    REQUIRE(df.nrows() == 1);
}

// setting and getting a value in a dataframe
TEST_CASE("setting and getting values", "[dataframe]") {
    Schema s("ISDB");
    DataFrame df(s);
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
    df.set(2, 88, (double)11.0);
    df.set(3, 76, true);

    REQUIRE(df.nrows() == 100);
    REQUIRE(df.ncols() == 4);

    // test get_int and set
    REQUIRE(df.get_int(0, 19) == 11);

    // test get_bool and set
    REQUIRE(df.get_bool(3, 76));

    // test get_double and set
    REQUIRE(double_equal(df.get_double(2, 88), 11.0));

    // test get_string and set
    REQUIRE(df.get_string(1, 94)->equals(str2));

    delete str;
    delete str2;
}

// tests that running pmap works with equally divided dataframe
TEST_CASE("pmap works evenly distributed", "[dataframe]") {
    Schema s("ISDB");
    DataFrame df(s);
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

    df.set(0, 99, 11);
    df.set(1, 99, str2);
    df.set(2, 99, (double)11.0);
    df.set(3, 99, true);

    REQUIRE(df.nrows() == 100);
    REQUIRE(df.ncols() == 4);

    MaxRower2 mr = MaxRower2();
    df.pmap(mr);

    REQUIRE(mr.maxInt == 11);
    REQUIRE(mr.maxStr->equals(str2));
    REQUIRE(double_equal(mr.maxFloat, 11.0));
    REQUIRE(mr.maxBool);

    delete str;
    delete str2;
}

// tests that running pmap works with unequally divided dataframe
TEST_CASE("pmap works unevenly distributed", "[dataframe]") {
    Schema s("ISDB");
    DataFrame df(s);
    Row r = Row(df.get_schema());
    String* str = new String("Test");
    String* str2 = new String("ZZZZZZZZ");
    for (int i = 0; i < 101; i++) {
        r.set(0, 8);
        r.set(1, str);
        r.set(2, 8.0f);
        r.set(3, false);
        df.add_row(r);
    }

    df.set(0, 100, 11);
    df.set(1, 100, str2);
    df.set(2, 100, (double)11.0);
    df.set(3, 100, true);
    MaxRower2 mr = MaxRower2();
    df.pmap(mr);

    REQUIRE(mr.maxInt == 11);
    REQUIRE(mr.maxStr->equals(str2));
    REQUIRE(double_equal(mr.maxFloat, 11.0));
    REQUIRE(mr.maxBool);

    delete str;
    delete str2;
}

// test fromArray methods
TEST_CASE("fromArray for all types", "[dataframe]") {
    KDStore kd;
    Key doubles("doubles");
    size_t SZ = 10;
    double* double_vals = new double[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        double_vals[i] = i;
    }
    DataFrame* df1 = DataFrame::fromArray(&doubles, &kd, SZ, double_vals);
    DataFrame* df1_copy = kd.get(doubles);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(double_equal(df1_copy->get_double(0, i), double_vals[i]));
    }

    Key ints("ints");
    int* int_vals = new int[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        int_vals[i] = i;
    }
    DataFrame* df2 = DataFrame::fromArray(&ints, &kd, SZ, int_vals);
    DataFrame* df2_copy = kd.get(ints);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(df2_copy->get_int(0, i) == i);
    }

    Key bools("bools");
    bool* bool_vals = new bool[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        bool_vals[i] = true;
    }
    DataFrame* df3 = DataFrame::fromArray(&bools, &kd, SZ, bool_vals);
    DataFrame* df3_copy = kd.get(bools);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(df3_copy->get_bool(0, i));
    }

    Key str("strings");
    String* test = new String("Test");
    String** str_vals = new String*[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        str_vals[i] = test;
    }
    DataFrame* df4 = DataFrame::fromArray(&str, &kd, SZ, str_vals);
    DataFrame* df4_copy = kd.get(str);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(df4_copy->get_string(0, i)->equals(test));
    }

    delete df1;
    delete df1_copy;
    delete df2;
    delete df2_copy;
    delete df3;
    delete df3_copy;
    delete df4;
    delete df4_copy;
    delete double_vals;
    delete int_vals;
    delete bool_vals;
    delete str_vals;
    delete test;
}

// test fromScalar methods
TEST_CASE("fromScalar for all types", "[dataframe]") {
    KDStore kd;
    Key doubles("doubles");
    double double_val = 4.5;
    DataFrame* df1 = DataFrame::fromScalar(&doubles, &kd, double_val);
    DataFrame* df1_copy = kd.get(doubles);
    REQUIRE(double_equal(df1_copy->get_double(0, 0), double_val));

    Key ints("ints");
    int int_val = -3;
    DataFrame* df2 = DataFrame::fromScalar(&ints, &kd, int_val);
    DataFrame* df2_copy = kd.get(ints);
    REQUIRE(df2_copy->get_int(0, 0) == int_val);

    Key bools("bools");
    DataFrame* df3 = DataFrame::fromScalar(&bools, &kd, true);
    DataFrame* df3_copy = kd.get(bools);
    REQUIRE(df3_copy->get_bool(0, 0));

    Key strs("strings");
    String test("Test");
    DataFrame* df4 = DataFrame::fromScalar(&strs, &kd, &test);
    DataFrame* df4_copy = kd.get(strs);
    REQUIRE(df3_copy->get_string(0, 0)->equals(&test));
}
