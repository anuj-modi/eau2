#pragma once
#include <arpa/inet.h>

#include "util/object.h"
#include "util/serial.h"
#include "util/string.h"

/**
 * A class which helps convert between address string and byte representations
 */
class Address : public Object {
   public:
    String *ip_str_;
    uint32_t ip_bytes_;
    uint16_t port_;

    /**
     * Creates an instance of address.
     * @arg ip  string literal representation of an ip
     */
    Address(const char *ip, uint16_t port) : Object() {
        ip_str_ = new String(ip);
        struct sockaddr_in addr;
        assert(inet_pton(AF_INET, ip, &addr.sin_addr) == 1);
        ip_bytes_ = addr.sin_addr.s_addr;
        port_ = port;
    }

    /**
     * Creates an instance of address.
     * @arg ip  string class representation of an ip
     */
    Address(String *ip, uint16_t port) : Address(ip->c_str(), port) {}

    /**
     * Creates an instance of address.
     * @arg addr  byte form of an ip address
     */
    Address(uint32_t addr, uint16_t port) : Object() {
        ip_bytes_ = addr;
        char buf[INET_ADDRSTRLEN];
        assert(inet_ntop(AF_INET, &addr, buf, sizeof(buf)) != nullptr);
        ip_str_ = new String(buf);
        port_ = port;
    }

    /**
     * Creates an instance of address.
     * @arg addr  byte representation of an ip address w/ POSIX
     */
    Address(struct sockaddr_in addr) : Address(addr.sin_addr.s_addr, addr.sin_port) {}

    Address(Address *other) : Object() {
        ip_str_ = new String(other->as_str()->c_str());
        ip_bytes_ = other->ip_bytes();
        port_ = other->port();
    }

    Address(Deserializer *d) : Object() {
        ip_bytes_ = d->get_uint32_t();
        char buf[INET_ADDRSTRLEN];
        assert(inet_ntop(AF_INET, &ip_bytes_, buf, sizeof(buf)) != nullptr);
        ip_str_ = new String(buf);
        port_ = d->get_uint16_t();
    }

    /**
     * Deconstructs an address.
     */
    virtual ~Address() {
        delete ip_str_;
    }

    /**
     * Gets ip address in bytes.
     * @return byte version of ip address
     */
    uint32_t ip_bytes() {
        return ip_bytes_;
    }

    /**
     * Gets string class version of ip
     * @return string with ip
     */
    String *as_str() {
        return ip_str_;
    }

    /**
     * Gets port number.
     * @return port number of address
     */
    uint16_t port() {
        return port_;
    }

    void serialize(Serializer *s) {
        s->add_uint32_t(ip_bytes_);
        s->add_uint16_t(port_);
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object *other) {
        if (other == this) {
            return true;
        }
        Address *o = dynamic_cast<Address *>(other);
        if (o == nullptr) {
            return false;
        }
        return ip_str_->equals(o->ip_str_) && ip_bytes_ == o->ip_bytes_ && port_ == o->port_;
    }
};
