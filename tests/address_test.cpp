#include "network/address.h"

#include <arpa/inet.h>

#include "catch.hpp"
#include "util/serial.h"

TEST_CASE("make address from string") {
    String s("192.168.1.1");
    Address a("192.168.1.1", 5555);

    REQUIRE(s.equals(a.as_str()));
    REQUIRE(a.port() == 5555);

    struct sockaddr_in addr;
    REQUIRE(inet_pton(AF_INET, s.c_str(), &addr.sin_addr) == 1);
    REQUIRE(addr.sin_addr.s_addr == a.ip_bytes());
}

TEST_CASE("make address from bytes") {
    String s("192.168.1.1");

    struct sockaddr_in addr;
    REQUIRE(inet_pton(AF_INET, s.c_str(), &addr.sin_addr) == 1);
    Address a(addr.sin_addr.s_addr, 5555);

    REQUIRE(a.ip_bytes() == addr.sin_addr.s_addr);
    REQUIRE(a.port() == 5555);
    REQUIRE(a.as_str()->equals(&s));
}

TEST_CASE("make address from another address") {
    String s("192.168.1.1");
    Address a("192.168.1.1", 5555);

    REQUIRE(s.equals(a.as_str()));
    REQUIRE(a.port() == 5555);

    struct sockaddr_in addr;
    REQUIRE(inet_pton(AF_INET, s.c_str(), &addr.sin_addr) == 1);
    REQUIRE(addr.sin_addr.s_addr == a.ip_bytes());

    Address b(a.ip_bytes(), 5555);

    REQUIRE(b.equals(&a));
}

TEST_CASE("test_serialize_address", "[serialize][address]") {
    Address a("192.168.1.10", 8080);
    Serializer s;
    a.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());

    uint32_t ip = d.get_uint32_t();
    uint16_t port = d.get_uint16_t();
    REQUIRE(ip == a.ip_bytes());
    REQUIRE(port == a.port());
    REQUIRE(d.bytes_remaining_ == 0);

    Address b(ip, port);
    REQUIRE(a.equals(&b));
}

TEST_CASE("test_serialize_deserialize_address", "[serialize][deserialize][address]") {
    Address a("192.168.1.10", 8080);
    Serializer s;
    a.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());
    Address a2(&d);
    REQUIRE(a.equals(&a2));
}
