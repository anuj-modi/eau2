#include "../src/dataframe.h"
#include "catch.hpp"

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
