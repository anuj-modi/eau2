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
