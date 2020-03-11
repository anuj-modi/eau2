#pragma once
#include "array.h"
#include "network.h"
#include "object.h"
#include "serial.h"

/**
 * Represents a message type to pass over a network.
 */
// enum class MsgType : int { REGISTER, KILL, DIRECTORY, STATUS };
enum class MsgType : int {
    ACK,
    NACK,
    PUT,
    REPLY,
    GET,
    WAITANDGET,
    STATUS,
    KILL,
    REGISTER,
    DIRECTORY
};

/**
 * Represents a message to send over a network.
 */
class Message : public Object {
   public:
    MsgType kind_;   // the message kind
    size_t sender_;  // the index of the sender node
    size_t target_;  // the index of the receiver node
    size_t id_;      // an id t unique within the node

    /**
     * Creates an instance of message.
     * @arg kind  the type of message
     */
    Message(MsgType kind) {
        kind_ = kind;
        sender_ = 0;
        target_ = 0;
        id_ = 0;
    }

    Message(Deserializer* d) {
        kind_ = d->get_msg_type();
        sender_ = d->get_size_t();
        target_ = d->get_size_t();
        id_ = d->get_size_t();
    }

    Message(MsgType t, Deserializer* d) {
        kind_ = t;
        sender_ = d->get_size_t();
        target_ = d->get_size_t();
        id_ = d->get_size_t();
    }

    /**
     * Deconstructs an instance of message.
     */
    virtual ~Message() {}

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        s->add_msg_type(kind_);
        s->add_size_t(sender_);
        s->add_size_t(target_);
        s->add_size_t(id_);
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Message* o = dynamic_cast<Message*>(other);
        if (o == nullptr) {
            return false;
        }
        if (kind_ == o->kind_ && sender_ == o->sender_ && target_ == o->target_ && id_ == o->id_) {
            return true;
        }
        return false;
    }
};

/**
 * Represents a register message (client -> server)
 */
class Register : public Message {
   public:
    Address* client_addr_;

    /**
     * Creates instance of a register message.
     * @arg addr  the address of the client that wants to register.
     */
    Register(Address* addr) : Message(MsgType::REGISTER) {
        assert(addr != nullptr);
        client_addr_ = addr;
    }

    Register(Deserializer* d) : Message(MsgType::REGISTER, d) {
        client_addr_ = new Address(d->get_uint32_t());
    }

    /**
     * Deconstructs an instance of a register message.
     */
    virtual ~Register() {
        delete client_addr_;
    }

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        s->add_uint32_t(client_addr_->as_bytes());
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Register* o = dynamic_cast<Register*>(other);
        if (o == nullptr) {
            return false;
        }
        if (kind_ == o->kind_ && sender_ == o->sender_ && target_ == o->target_ && id_ == o->id_ &&
            client_addr_->as_bytes() == o->client_addr_->as_bytes()) {
            return true;
        }
        return false;
    }
};

/**
 * Represents a directory message (server -> clients)
 */
class Directory : public Message {
   public:
    Array<Address*>* client_addrs_;

    /**
     * Creates instance of a directory message.
     * @arg addrs  the addresss of the clients that are registered
     */
    Directory(Array* addrs) : Message(MsgType::DIRECTORY) {
        assert(addrs != nullptr);
        client_addrs_ = addrs;
    }

    Directory(Deserializer* d) : Message(MsgType::DIRECTORY, d) {
        client_addrs_ = new Array<Address*>();
        size_t num_addrs = d->get_size_t();
        for (size_t i = 0; i < num_addrs; i++) {
            client_addrs_->push_back(new Address(d->get_uint32_t()));
        }
    }

    /**
     * Deconstructs an instance of a register message.
     */
    virtual ~Directory() {
        // delete client_addrs_;
    }

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        s->add_size_t(client_addrs_->size());
        for (size_t i = 0; i < client_addrs_->size(); i++) {
            Address* addr = static_cast<Address*>(client_addrs_->get(i));
            s->add_uint32_t(addr->as_bytes());
        }
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Directory* o = dynamic_cast<Directory*>(other);
        if (o == nullptr) {
            return false;
        }
        if (kind_ == o->kind_ && sender_ == o->sender_ && target_ == o->target_ && id_ == o->id_ &&
            client_addrs_->equals(o->client_addrs_)) {
            return true;
        }
        return false;
    }
};

/**
 * Represents a status message. (client -> client)
 */
class Status : public Message {
   public:
    String* message_;

    /**
     * Creates an instance of a status message.
     * @arg message  the status message being sent
     *
     */
    Status(const char* message) : Message(MsgType::STATUS) {
        message_ = new String(message);
    }

    Status(Deserializer* d) : Message(MsgType::STATUS, d) {
        message_ = d->get_string();
    }

    /**
     * Deconstruct an instance of status message.
     */
    virtual ~Status() {
        delete message_;
    }

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        s->add_string(message_);
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Status* o = dynamic_cast<Status*>(other);
        if (o == nullptr) {
            return false;
        }
        if (kind_ == o->kind_ && sender_ == o->sender_ && target_ == o->target_ && id_ == o->id_ &&
            message_->equals(o->message_)) {
            return true;
        }
        return false;
    }
};

class Ack : public Message {
   public:
    Ack() : Message(MsgType::ACK) {}
    Ack(Deserializer* d) : Message(MsgType::ACK, d) {}

    virtual ~Ack() {}

    virtual void serialize(Serializer* s) {
        Message::serialize(s);
    }
};
