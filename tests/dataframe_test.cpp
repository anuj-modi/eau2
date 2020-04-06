#include "dataframe/dataframe.h"

#include <map>
#include <string>

#include "catch.hpp"
#include "dataframe/visitor.h"
#include "store/kdstore.h"

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

// test get_schema
TEST_CASE("get schema from a dataframe", "[dataframe]") {
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    ic->push_back(5);
    DataFrame df(ic, &kv);
    Schema s = df.get_schema();
    REQUIRE(s.col_type(0) == 'I');
    REQUIRE(s.length() == 1);
    REQUIRE(s.width() == 1);
}

// test getting a value in a dataframe
TEST_CASE("getting values out of dataframe", "[dataframe]") {
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    BoolColumn* bc = new BoolColumn(&kv);
    StringColumn* sc = new StringColumn(&kv);
    DoubleColumn* dc = new DoubleColumn(&kv);
    String* str = new String("Test");
    for (int i = 0; i < 100; i++) {
        ic->push_back(i);
        bc->push_back(true);
        sc->push_back(str);
        dc->push_back(i);
    }
    std::vector<Column*> cs = std::vector<Column*>();
    cs.push_back(ic);
    cs.push_back(bc);
    cs.push_back(sc);
    cs.push_back(dc);
    DataFrame df(cs, &kv);

    REQUIRE(df.nrows() == 100);
    REQUIRE(df.ncols() == 4);

    // test get_int and set
    REQUIRE(df.get_int(0, 19) == 19);

    // test get_bool and set
    REQUIRE(df.get_bool(1, 76));

    // test get_string and set
    String* result = df.get_string(2, 94);
    REQUIRE(result->equals(str));

    // test get_double and set
    REQUIRE(double_equal(df.get_double(3, 88), 88.0));

    delete result;
    delete str;
}

// test col_type method
TEST_CASE("get the type of column in data frame", "[dataframe]") {
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    BoolColumn* bc = new BoolColumn(&kv);
    StringColumn* sc = new StringColumn(&kv);
    DoubleColumn* dc = new DoubleColumn(&kv);
    std::vector<Column*> cs = std::vector<Column*>();
    cs.push_back(ic);
    cs.push_back(bc);
    cs.push_back(sc);
    cs.push_back(dc);
    DataFrame df(cs, &kv);
    REQUIRE(df.col_type(0) == 'I');
    REQUIRE(df.col_type(1) == 'B');
    REQUIRE(df.col_type(2) == 'S');
    REQUIRE(df.col_type(3) == 'D');
}

// test fill_row method
TEST_CASE("fill row with data frame data", "[dataframe]") {
    KVStore kv;
    IntColumn* ic = new IntColumn(&kv);
    BoolColumn* bc = new BoolColumn(&kv);
    StringColumn* sc = new StringColumn(&kv);
    DoubleColumn* dc = new DoubleColumn(&kv);
    String* str = new String("Test");
    for (int i = 0; i < 10; i++) {
        ic->push_back(i);
        bc->push_back(true);
        sc->push_back(str);
        dc->push_back(i);
    }
    std::vector<Column*> cs = std::vector<Column*>();
    cs.push_back(ic);
    cs.push_back(bc);
    cs.push_back(sc);
    cs.push_back(dc);
    DataFrame df(cs, &kv);
    Row r(df.get_schema());
    df.fill_row(8, r);
    REQUIRE(r.get_int(0) == 8);
    REQUIRE(r.get_bool(1));
    REQUIRE(r.get_string(2)->equals(str));
    REQUIRE(double_equal(r.get_double(3), 8.0));
    df.fill_row(9, r);
    REQUIRE(r.get_int(0) == 9);
    REQUIRE(r.get_bool(1));
    REQUIRE(r.get_string(2)->equals(str));
    REQUIRE(double_equal(r.get_double(3), 9.0));

    delete str;
}

// test fromArray methods
TEST_CASE("fromArray for all types", "[dataframe][kdstore]") {
    KVStore kv;
    KDStore kd(&kv);
    Key* doubles = new Key("doubles");
    size_t SZ = 10;
    double* double_vals = new double[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        double_vals[i] = i;
    }
    delete DataFrame::fromArray(doubles, &kd, SZ, double_vals);
    Key db("doubles");
    DataFrame* df1_copy = kd.get(db);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(double_equal(df1_copy->get_double(0, i), double_vals[i]));
    }

    Key* ints = new Key("ints");
    int* int_vals = new int[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        int_vals[i] = i;
    }
    delete DataFrame::fromArray(ints, &kd, SZ, int_vals);

    Key in("ints");
    DataFrame* df2_copy = kd.get(in);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(df2_copy->get_int(0, i) == i);
    }

    Key* bools = new Key("bools");
    bool* bool_vals = new bool[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        bool_vals[i] = true;
    }
    delete DataFrame::fromArray(bools, &kd, SZ, bool_vals);

    Key b("bools");
    DataFrame* df3_copy = kd.get(b);
    for (size_t i = 0; i < SZ; ++i) {
        REQUIRE(df3_copy->get_bool(0, i));
    }

    Key* str = new Key("strings");
    String* test = new String("Test");
    String** str_vals = new String*[SZ];
    for (size_t i = 0; i < SZ; ++i) {
        str_vals[i] = test;
    }
    delete DataFrame::fromArray(str, &kd, SZ, str_vals);

    Key s("strings");
    String* result;
    DataFrame* df4_copy = kd.get(s);
    for (size_t i = 0; i < SZ; ++i) {
        result = df4_copy->get_string(0, i);
        REQUIRE(result->equals(test));
        delete result;
    }

    delete df1_copy;
    delete df2_copy;
    delete df3_copy;
    delete df4_copy;
    delete[] double_vals;
    delete[] int_vals;
    delete[] bool_vals;
    delete[] str_vals;
    delete test;
    delete ints;
    delete bools;
    delete str;
    delete doubles;
}

// test fromScalar methods
TEST_CASE("fromScalar for all types", "[dataframe][kdstore]") {
    KVStore kv;
    KDStore kd(&kv);
    Key* doubles = new Key("doubles");
    double double_val = 4.5;
    delete DataFrame::fromScalar(doubles, &kd, double_val);

    Key db("doubles");
    DataFrame* df1_copy = kd.get(db);
    REQUIRE(double_equal(df1_copy->get_double(0, 0), double_val));

    Key* ints = new Key("ints");
    int int_val = -3;
    delete DataFrame::fromScalar(ints, &kd, int_val);

    Key in("ints");
    DataFrame* df2_copy = kd.get(in);
    REQUIRE(df2_copy->get_int(0, 0) == int_val);

    Key* bools = new Key("bools");
    delete DataFrame::fromScalar(bools, &kd, true);

    Key b("bools");
    DataFrame* df3_copy = kd.get(b);
    REQUIRE(df3_copy->get_bool(0, 0));

    Key* strs = new Key("strings");
    String test("Test");
    delete DataFrame::fromScalar(strs, &kd, &test);

    Key s("strings");
    DataFrame* df4_copy = kd.get(s);
    String* result = df4_copy->get_string(0, 0);
    REQUIRE(result->equals(&test));

    delete df1_copy;
    delete df2_copy;
    delete df3_copy;
    delete df4_copy;
    delete result;
    delete ints;
    delete bools;
    delete strs;
    delete doubles;
}

// test fromSorFile method
TEST_CASE("create data frame from sor file", "[dataframe][kdstore]") {
    KVStore kv;
    KDStore kd(&kv);
    Key k("data");
    DataFrame* df = DataFrame::fromSorFile(&k, &kd, "./data/data4.sor");
    DataFrame* df_copy = kd.get(k);

    REQUIRE(df->ncols() == 4);
    REQUIRE(df->nrows() == 672);
    REQUIRE(df->get_bool(0, 486));
    REQUIRE(df->get_int(1, 654) == -11);
    REQUIRE(double_equal(df->get_double(2, 83), -17.5));
    REQUIRE(df_copy->ncols() == 4);
    REQUIRE(df_copy->nrows() == 672);
    REQUIRE(df_copy->get_bool(0, 486));
    REQUIRE(df_copy->get_int(1, 654) == -11);
    REQUIRE(double_equal(df_copy->get_double(2, 83), -17.5));
    String* s = new String("0.4");
    String* s2 = df->get_string(3, 294);
    String* s3 = df_copy->get_string(3, 294);
    REQUIRE(s2->equals(s));
    REQUIRE(s3->equals(s));
    delete df;
    delete df_copy;
    delete s;
    delete s2;
    delete s3;
}

class Summer : public Writer {
   public:
    std::unordered_map<std::string, int>::iterator it_;
    std::unordered_map<std::string, int> map_;

    Summer(std::unordered_map<std::string, int>& map) : Writer() {
        map_ = map;
        it_ = map_.begin();
    }

    virtual ~Summer() {}

    /**
     * Visits the given row.
     * @arg r  the row
     */
    virtual void visit(Row& r) {
        String* key = new String(it_->first.c_str());
        r.set(0, key);
        r.set(1, it_->second);
        it_++;
    }

    /**
     * Marks when the writer is done visiting the data frame.
     * @return true if done
     */
    virtual bool done() {
        return it_ == map_.end();
    }
};

// test fromVisitor method
TEST_CASE("create df from map with fromVisitor", "[dataframe][kdstore]") {
    std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
    KVStore kv;
    KDStore kd(&kv);
    map["catch"] = 10;
    map["super"] = 24;
    map["monkey"] = 7;
    Summer s(map);
    Key cnts("counts");
    DataFrame* df = DataFrame::fromVisitor(&cnts, &kd, "SI", s);
    DataFrame* df_copy = kd.get(cnts);
    String* s1 = df->get_string(0, 0);
    String* s2 = df->get_string(0, 1);
    String* s3 = df->get_string(0, 2);
    String* s1_copy = df_copy->get_string(0, 0);
    String* s2_copy = df_copy->get_string(0, 1);
    String* s3_copy = df_copy->get_string(0, 2);

    REQUIRE(s1->equals(s1_copy));
    REQUIRE(s2->equals(s2_copy));
    REQUIRE(s3->equals(s3_copy));
    REQUIRE(df->get_int(1, 0) == df_copy->get_int(1, 0));
    REQUIRE(df->get_int(1, 1) == df_copy->get_int(1, 1));
    REQUIRE(df->get_int(1, 2) == df_copy->get_int(1, 2));

    delete df;
    delete df_copy;
    delete s1;
    delete s2;
    delete s3;
    delete s1_copy;
    delete s2_copy;
    delete s3_copy;
}

class Adder : public Reader {
   public:
    std::unordered_map<std::string, int> map_;  // String to Num map;  Num holds an int

    Adder(std::unordered_map<std::string, int>& map) : Reader() {
        map_ = map;
    }

    void visit(Row& r) override {
        std::string w = std::string(r.get_string(0)->c_str());
        if (map_.find(w) == map_.end()) {
            map_[w] = 1;
        } else {
            map_[w] += 1;
        }
    }
};

test local_map method
TEST_CASE("adder with local_map on data frame", "[dataframe][kdstore]") {
    String* hello = new String("hello");
    String* world = new String("world");
    String* potato = new String("potato");
    KVStore kv;
    KDStore kd(&kv);
    StringColumn* sc = new StringColumn(&kv);
    for (size_t i = 0; i < 138; i++) {
        sc->push_back(hello);
    }
    sc->push_back(world);
    for (size_t i = 0; i < 2; i++) {
        sc->push_back(potato);
    }
    sc->push_back(hello);
    Key not_included("not included", 1);
    sc->segments_[0] = not_included;
    DataFrame df(sc, &kv);
    std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
    Adder add(map);
    df.local_map(add);

    REQUIRE(add.map_[std::string("hello")] == 11);
    REQUIRE(add.map_[std::string("world")] == 1);
    REQUIRE(add.map_[std::string("potato")] ==  2);

    delete hello;
    delete world;
    delete potato;
}

// // TODO test map method
