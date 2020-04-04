#include "network/message.h"

#include <unordered_map>

#include "catch.hpp"
#include "store/key.h"
#include "store/value.h"
#include "util/serial.h"

TEST_CASE("test_serialize_deserialize_base_message", "[message][serialize][deserialize]") {
    Message m(MsgType::STATUS);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());

    Message m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);
}

TEST_CASE("test_serialize_deserialize_put_message", "[message][serialize][deserialize]") {
    Key k("adsf");
    String st("serialized data");
    Value* v = new Value(st.c_str(), st.size());
    Put m(k, v);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::PUT);
    Put m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);

    REQUIRE(m2.k_ == m.k_);
    REQUIRE(m2.v_->equals(m.v_));
}

TEST_CASE("test_serialize_deserialize_get_message", "[message][serialize][deserialize]") {
    Key k("adsf");
    Get m(k);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::GET);
    Get m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);

    REQUIRE(m2.k_ == m.k_);
}

TEST_CASE("test_serialize_deserialize_wait_and_get_message", "[message][serialize][deserialize]") {
    Key k("adsf");
    WaitAndGet m(k);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::WAITANDGET);
    WaitAndGet m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);

    REQUIRE(m2.k_ == m.k_);
}

TEST_CASE("test_serialize_deserialize_reply_message", "[message][serialize][deserialize]") {
    String st("serialized data");
    Value* v = new Value(st.c_str(), st.size());
    Reply m(v);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::REPLY);
    Reply m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);

    REQUIRE(m2.v_->equals(m.v_));
}

TEST_CASE("test_serialize_deserialize_register_message", "[message][serialize][deserialize]") {
    Address* a = new Address("192.168.1.10", 8080);
    Register m(a, 1);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::REGISTER);
    Register m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);

    REQUIRE(m2.client_addr_->equals(m2.client_addr_));
}

TEST_CASE("test_serialize_deserialize_directory_message", "[message][serialize][deserialize]") {
    Address* a = new Address("192.168.1.10", 8080);
    Address* b = new Address("192.168.1.100", 5555);
    std::unordered_map<size_t, Address*> addrs;
    addrs[0] = a;
    addrs[1] = b;
    Directory m(addrs);
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::DIRECTORY);
    Directory m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);
    REQUIRE(m2.client_addrs_.size() == m.client_addrs_.size());

    for (size_t i = 0; i < m2.client_addrs_.size(); i++) {
        REQUIRE(m2.client_addrs_[i]->equals(m.client_addrs_[i]));
        delete m2.client_addrs_[i];
        delete m.client_addrs_[i];
    }
}

TEST_CASE("test_serialize_deserialize_status_message", "[message][serialize][deserialize]") {
    Status m("my new status");
    m.sender_ = 123;
    m.target_ = 456;
    m.id_ = 789;
    Serializer s;
    m.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    REQUIRE(d.get_msg_type() == MsgType::STATUS);
    Status m2(&d);

    REQUIRE(m2.kind_ == m.kind_);
    REQUIRE(m2.sender_ == m.sender_);
    REQUIRE(m2.target_ == m.target_);
    REQUIRE(m2.id_ == m.id_);

    REQUIRE(m2.message_->equals(m2.message_));
}
