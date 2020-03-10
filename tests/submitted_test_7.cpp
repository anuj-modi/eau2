#include "../src/dataframe.h"
#include "catch.hpp"

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
