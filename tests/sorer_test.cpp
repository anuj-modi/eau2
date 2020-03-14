#include "../src/dataframe.h"
#include "../src/sorer/parser.h"
#include "catch.hpp"

// /**
//  * Determine if these two doubles are equal with respect to eps.
//  * @param f1 the first double to compare.
//  * @param f2 the second double to compare.
//  */
// static bool double_equal(double f1, double f2) {
//     double eps = 0.0000001;
//     if (f1 > f2) {
//         return f1 - f2 < eps;
//     } else {
//         return f2 - f1 < eps;
//     }
// }

TEST_CASE("test_simple_sor", "[sor]") {
    FILE* file = fopen("../data/data1.sor", "r");
    SorParser parser(file);
    parser.guessSchema();
    parser.parseFile();
    ColumnSet* cols = parser.getColumnSet();
    REQUIRE(cols->getLength() == 4);

    Schema s;
    s.add_row();
    REQUIRE(s.length() == 1);

    DataFrame df(s);
    REQUIRE(df.nrows() == 1);

    for (size_t i = 0; i < cols->getLength(); i++) {
        REQUIRE(df.nrows() == 1);
        df.add_column(cols->getColumn(i));
    }

    REQUIRE(df.ncols() == 4);
    REQUIRE(df.nrows() == 1);
    REQUIRE(df.get_schema().width() == 4);
    REQUIRE(df.get_schema().length() == 1);
    REQUIRE(df.get_schema().col_type(0) == 'B');
    REQUIRE(df.get_schema().col_type(1) == 'I');
    REQUIRE(df.get_schema().col_type(2) == 'S');
    REQUIRE(df.get_schema().col_type(3) == 'I');
}
