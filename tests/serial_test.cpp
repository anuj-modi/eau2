#include "util/serial.h"
#include <float.h>
#include <limits.h>
#include "catch.hpp"
#include "util/string.h"
#include "util/array.h"

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

TEST_CASE("test_serialize_deserialize_int", "[serialize][deserialize]") {
    Serializer s;
    s.add_int(0);
    s.add_int(1);
    s.add_int(INT_MAX);
    s.add_int(-12345);
    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_int() == 0);
    REQUIRE(d.get_int() == 1);
    REQUIRE(d.get_int() == INT_MAX);
    REQUIRE(d.get_int() == -12345);
}

TEST_CASE("test_serialize_deserialize_bool", "[serialize][deserialize]") {
    Serializer s;
    s.add_bool(true);
    s.add_bool(false);
    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_bool());
    REQUIRE_FALSE(d.get_bool());
}

TEST_CASE("test_serialize_deserialize_double", "[serialize][deserialize]") {
    Serializer s;
    s.add_double(0);
    s.add_double(1.1111111);
    s.add_double(DBL_MAX);
    s.add_double(-3.1415926);
    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(double_equal(d.get_double(), 0));
    REQUIRE(double_equal(d.get_double(), 1.1111111));
    REQUIRE(double_equal(d.get_double(), DBL_MAX));
    REQUIRE(double_equal(d.get_double(), -3.1415926));
}

TEST_CASE("test_serialize_deserialize_size_t", "[serialize][deserialize]") {
    Serializer s;
    s.add_size_t(0);
    s.add_size_t(SIZE_MAX);
    s.add_size_t(1234);
    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_size_t() == 0);
    REQUIRE(d.get_size_t() == SIZE_MAX);
    REQUIRE(d.get_size_t() == 1234);
}

TEST_CASE("test_serialize_deserialize_uint_32_t", "[serialize][deserialize]") {
    Serializer s;
    s.add_size_t(0);
    s.add_size_t(UINT32_MAX);
    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_size_t() == 0);
    REQUIRE(d.get_size_t() == UINT32_MAX);
}

TEST_CASE("test_serialize_deserialize_buffer", "[serialize][deserialize][string]") {
    Serializer s;
    s.add_buffer("TESTING", sizeof("TESTING"));
    Deserializer d(s.get_bytes(), s.size());
    char* buffer = d.get_buffer(sizeof("TESTING"));
    String* str = new String(buffer);
    String* old_str = new String("TESTING");
    REQUIRE(str->equals(old_str));

    delete[] buffer;
    delete str;
    delete old_str;
}

TEST_CASE("test_serialize_deserialize_string", "[serialize][deserialize][string]") {
    String* h1 = new String("hello there");
    String* h2 = new String("why howdy!");
    String* h3 = new String("greetings everyone");

    Serializer s;
    s.add_string(h1);
    s.add_string(h2);
    s.add_string(h3);

    Deserializer d(s.get_bytes(), s.size());
    String* h1d = d.get_string();
    String* h2d = d.get_string();
    String* h3d = d.get_string();
    REQUIRE(h1d->equals(h1));
    REQUIRE(h2d->equals(h2));
    REQUIRE(h3d->equals(h3));

    delete h1;
    delete h2;
    delete h3;
    delete h1d;
    delete h2d;
    delete h3d;
}

TEST_CASE("test serialize deserialize string array",
          "[serialize][deserialize][array][string]") {
    StringArray* strs = new StringArray(5);
    String* h1 = new String("hello there");
    String* h2 = new String("why howdy!");
    strs->push_back(h1);
    strs->push_back(h2);
    Serializer* s = new Serializer();
    strs->serialize(s);

    Deserializer* d = new Deserializer(s->get_bytes(), s->size());
    StringArray* strs_copy = new StringArray(d);

    REQUIRE(strs_copy->get_string(0)->equals(h1));
    REQUIRE(strs_copy->get_string(1)->equals(h2));

    delete strs;
    delete strs_copy;
    delete s;
    delete d;
}

TEST_CASE("test serialize deserialize double array", "[serialize][deserialize][array]") {
    DoubleArray* doubles = new DoubleArray(5);
    doubles->push_back(1.2);
    doubles->push_back(-4.5);
    Serializer* s = new Serializer();
    doubles->serialize(s);
    Deserializer* d = new Deserializer(s->get_bytes(), s->size());
    DoubleArray* doubles_copy = new DoubleArray(d);

    REQUIRE(double_equal(doubles_copy->get_double(0), doubles->get_double(0)));
    REQUIRE(double_equal(doubles_copy->get_double(1), doubles->get_double(1)));

    delete doubles;
    delete doubles_copy;
    delete s;
    delete d;
}
