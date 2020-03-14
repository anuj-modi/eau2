#include "../src/dataframe.h"
#include "../src/sorer/parser.h"
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

TEST_CASE("test simple sor", "[sor]") {
    FILE* file = fopen("../data/data1.sor", "r");
    SorParser parser(file);
    parser.guessSchema();
    parser.parseFile();
    ColumnSet* cols = parser.getColumnSet();
    String* result_str = new String("string with > inside ");
    DataFrame df(cols->getColumns());

    REQUIRE(df.ncols() == 5);
    REQUIRE(df.nrows() == 1);
    REQUIRE(df.get_bool(0, 0) == true);
    REQUIRE(df.get_int(1, 0) == 2);
    REQUIRE(df.get_string(2, 0)->equals(result_str));
    REQUIRE(df.get_int(3, 0) == 4);
    REQUIRE(double_equal(df.get_double(4, 0), -3.14));

    delete df.get_string(2, 0);
    delete result_str;
    fclose(file);
}

TEST_CASE("test single value in sor file", "[sor]") {
    FILE* file = fopen("../data/data1.sor", "r");
    SorParser parser(file);
    parser.guessSchema();
    parser.parseFile();
    ColumnSet* cols = parser.getColumnSet();
    DataFrame df(cols->getColumns());

    REQUIRE(df.ncols() == 1);
    REQUIRE(df.nrows() == 1);
    REQUIRE(df.get_int(0, 0) == -5);
}
