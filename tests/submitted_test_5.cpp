#include "../src/dataframe.h"
#include "catch.hpp"

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
