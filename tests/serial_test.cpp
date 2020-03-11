#include "../src/serial.h"
#include "../src/message.h"
#include "../src/string.h"
#include "catch.hpp"

static bool double_equal(double f1, double f2) {
    float eps = 0.0000001;
    if (f1 > f2) {
        return f1 - f2 < eps;
    } else {
        return f2 - f1 < eps;
    }
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

TEST_CASE("test_serialize_deserialize_message_type", "[serialize][deserialize][message_type]") {
    MsgType types[] = {MsgType::ACK,      MsgType::NACK,       MsgType::PUT,    MsgType::REPLY,
                       MsgType::GET,      MsgType::WAITANDGET, MsgType::STATUS, MsgType::KILL,
                       MsgType::REGISTER, MsgType::DIRECTORY};
    Serializer s;
    for (size_t i = 0; i < 10; i++) {
        s.add_msg_type(types[i]);
    }
    Deserializer d(s.get_bytes(), s.size());
    for (size_t i = 0; i < 10; i++) {
        REQUIRE(d.get_msg_type() == types[i]);
    }
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

TEST_CASE("test_serialize_deserialize_string_array", "[serialize][deserialize][array][string]") {
    StringArray* strs = new StringArray();
    String* h1 = new String("hello there");
    String* h2 = new String("why howdy!");
    strs->push_back(h1);
    strs->push_back(h2);
    Serializer* s = new Serializer();
    strs->serialize(s);
    Deserializer* d = new Deserializer(s->get_bytes(), s->size());
    StringArray* strs_copy = new StringArray(d);
    for (size_t i = 0; i < strs->size_; i++) {
        REQUIRE(strs->get(i)->equals(strs_copy->get(i)));
    }
    delete h1;
    delete h2;
    delete strs;
    delete strs_copy->get(0);
    delete strs_copy->get(1);
    delete strs_copy;
    delete s;
    delete d;
}

TEST_CASE("test_serialize_deserialize_double_array", "[serialize][deserialize][array]") {
    DoubleArray* doubles = new DoubleArray();
    doubles->push_back(1.2);
    doubles->push_back(-4.5);
    Serializer* s = new Serializer();
    doubles->serialize(s);
    Deserializer* d = new Deserializer(s->get_bytes(), s->size());
    DoubleArray* doubles_copy = new DoubleArray(d);
    for (size_t i = 0; i < doubles->size_; i++) {
        REQUIRE(double_equal(doubles->get(i), doubles_copy->get(i)));
    }
    delete doubles;
    delete doubles_copy;
    delete s;
    delete d;
}
