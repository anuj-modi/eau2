#include "../src/dataframe.h"
#include "catch.hpp"

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
