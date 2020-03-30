#include "dataframe/dataframe.h"
#include "catch.hpp"
#include "store/kdstore.h"

// test get_schema
TEST_CASE("get schema from a dataframe", "[dataframe]") {
    KVStore kv;
    IntColumn ic(&kv);
    ic.push_back(5);
    DataFrame df(&ic, &kv);
    Schema s = df.get_schema();
    REQUIRE(s.col_type(0) == 'S');
    REQUIRE(s.length() == 1);
    REQUIRE(s.width() == 1);
}

// getting a value in a dataframe
TEST_CASE("getting values out of dataframe", "[dataframe]") {
    Schema s("ISDB");
    KVStore kv;
    DataFrame df(s, &kv);
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
    String* result = df.get_string(1, 94);
    REQUIRE(result->equals(str2));

    delete result;
    delete str;
    delete str2;
}

// // test fromArray methods
// TEST_CASE("fromArray for all types", "[dataframe][kdstore]") {
//     KVStore kv;
//     KDStore kd(&kv);
//     Key* doubles = new Key("doubles");
//     size_t SZ = 10;
//     double* double_vals = new double[SZ];
//     for (size_t i = 0; i < SZ; ++i) {
//         double_vals[i] = i;
//     }
//     delete DataFrame::fromArray(doubles, &kd, SZ, double_vals);
//     Key db("doubles");
//     DataFrame* df1_copy = kd.get(db);
//     for (size_t i = 0; i < SZ; ++i) {
//         REQUIRE(double_equal(df1_copy->get_double(0, i), double_vals[i]));
//     }

//     Key* ints = new Key("ints");
//     int* int_vals = new int[SZ];
//     for (size_t i = 0; i < SZ; ++i) {
//         int_vals[i] = i;
//     }
//     delete DataFrame::fromArray(ints, &kd, SZ, int_vals);

//     Key in("ints");
//     DataFrame* df2_copy = kd.get(in);
//     for (size_t i = 0; i < SZ; ++i) {
//         REQUIRE(df2_copy->get_int(0, i) == i);
//     }

//     Key* bools = new Key("bools");
//     bool* bool_vals = new bool[SZ];
//     for (size_t i = 0; i < SZ; ++i) {
//         bool_vals[i] = true;
//     }
//     delete DataFrame::fromArray(bools, &kd, SZ, bool_vals);

//     Key b("bools");
//     DataFrame* df3_copy = kd.get(b);
//     for (size_t i = 0; i < SZ; ++i) {
//         REQUIRE(df3_copy->get_bool(0, i));
//     }

//     Key* str = new Key("strings");
//     String* test = new String("Test");
//     String** str_vals = new String*[SZ];
//     for (size_t i = 0; i < SZ; ++i) {
//         str_vals[i] = test;
//     }
//     delete DataFrame::fromArray(str, &kd, SZ, str_vals);

//     Key s("strings");
//     String* result;
//     DataFrame* df4_copy = kd.get(s);
//     for (size_t i = 0; i < SZ; ++i) {
//         result = df4_copy->get_string(0, i);
//         REQUIRE(result->equals(test));
//         delete result;
//     }

//     delete df1_copy;
//     delete df2_copy;
//     delete df3_copy;
//     delete df4_copy;
//     delete[] double_vals;
//     delete[] int_vals;
//     delete[] bool_vals;
//     delete[] str_vals;
//     delete test;
//     delete ints;
//     delete bools;
//     delete str;
//     delete doubles;
// }

// // test fromScalar methods
// TEST_CASE("fromScalar for all types", "[dataframe][kdstore]") {
//     KVStore kv;
//     KDStore kd(&kv);
//     Key* doubles = new Key("doubles");
//     double double_val = 4.5;
//     delete DataFrame::fromScalar(doubles, &kd, double_val);

//     Key db("doubles");
//     DataFrame* df1_copy = kd.get(db);
//     REQUIRE(double_equal(df1_copy->get_double(0, 0), double_val));

//     Key* ints = new Key("ints");
//     int int_val = -3;
//     delete DataFrame::fromScalar(ints, &kd, int_val);

//     Key in("ints");
//     DataFrame* df2_copy = kd.get(in);
//     REQUIRE(df2_copy->get_int(0, 0) == int_val);

//     Key* bools = new Key("bools");
//     delete DataFrame::fromScalar(bools, &kd, true);

//     Key b("bools");
//     DataFrame* df3_copy = kd.get(b);
//     REQUIRE(df3_copy->get_bool(0, 0));

//     Key* strs = new Key("strings");
//     String test("Test");
//     delete DataFrame::fromScalar(strs, &kd, &test);

//     Key s("strings");
//     DataFrame* df4_copy = kd.get(s);
//     String* result = df4_copy->get_string(0, 0);
//     REQUIRE(result->equals(&test));

//     delete df1_copy;
//     delete df2_copy;
//     delete df3_copy;
//     delete df4_copy;
//     delete result;
//     delete ints;
//     delete bools;
//     delete strs;
//     delete doubles;
// }
