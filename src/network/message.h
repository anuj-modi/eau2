#pragma once
#include <unordered_map>
#include <vector>

#include "address.h"
#include "store/key.h"
#include "store/value.h"
#include "util/object.h"
#include "util/serial.h"

/**
 * Represents a message type to pass over a network.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
enum class MsgType : int { PUT, GET, WAITANDGET, REPLY, KILL, REGISTER, DIRECTORY, STATUS };

/**
 * Represents a message to send over a network.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
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
    Message(MsgType kind) : Object() {
        kind_ = kind;
        sender_ = 0;
        target_ = 0;
        id_ = 0;
    }

    Message(Deserializer* d) : Object() {
        kind_ = d->get_msg_type();
        sender_ = d->get_size_t();
        target_ = d->get_size_t();
        id_ = d->get_size_t();
    }

    Message(MsgType t, Deserializer* d) : Object() {
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

class Put : public Message {
   public:
    Key k_;
    Value* v_;

    Put(Key& k, Value* v) : Message(MsgType::PUT) {
        k_ = Key(k);
        v_ = v;
    }

    Put(Deserializer* d) : Message(MsgType::PUT, d) {
        k_ = Key(d);
        v_ = new Value(d);
    }

    /**
     * Deconstructs an instance of a put message.
     */
    virtual ~Put() {
        delete v_;
    }

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        k_.serialize(s);
        v_->serialize(s);
    }
};

class Get : public Message {
   public:
    Key k_;

    Get(Key& k) : Message(MsgType::GET), k_(k) {}

    Get(Deserializer* d) : Message(MsgType::GET, d) {
        k_ = Key(d);
    }

    /**
     * Deconstructs an instance of a get message.
     */
    virtual ~Get() {}

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        k_.serialize(s);
    }
};

class WaitAndGet : public Message {
   public:
    Key k_;

    WaitAndGet(Key& k) : Message(MsgType::WAITANDGET), k_(k) {}

    WaitAndGet(Deserializer* d) : Message(MsgType::WAITANDGET, d) {
        k_ = Key(d);
    }

    /**
     * Deconstructs an instance of a get message.
     */
    virtual ~WaitAndGet() {}

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        k_.serialize(s);
    }
};

class Reply : public Message {
   public:
    Value* v_;

    Reply(Value* v) : Message(MsgType::REPLY) {
        v_ = v;
    }

    Reply(Deserializer* d) : Message(MsgType::REPLY, d) {
        v_ = new Value(d);
    }

    /**
     * Deconstructs an instance of a Reply message.
     */
    virtual ~Reply() {
        delete v_;
    }

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        v_->serialize(s);
    }
};

/**
 * Represents a register message (client -> server)
 */
class Register : public Message {
   public:
    size_t node_num_;
    Address* client_addr_;

    /**
     * Creates instance of a register message.
     * @arg addr  the address of the client that wants to register.
     */
    Register(Address* addr, size_t node_num) : Message(MsgType::REGISTER) {
        assert(addr != nullptr);
        node_num_ = node_num;
        client_addr_ = addr;
    }

    Register(Deserializer* d) : Message(MsgType::REGISTER, d) {
        node_num_ = d->get_size_t();
        client_addr_ = new Address(d);
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
        s->add_size_t(node_num_);
        client_addr_->serialize(s);
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object* other) {
        assert(false);
        // if (other == this) {
        //     return true;
        // }
        // Register* o = dynamic_cast<Register*>(other);
        // if (o == nullptr) {
        //     return false;
        // }
        // if (kind_ == o->kind_ && sender_ == o->sender_ && target_ == o->target_ && id_ == o->id_
        // &&
        //     client_addr_->ip_bytes() == o->client_addr_->ip_bytes()) {
        //     return true;
        // }
        // return false;
    }
};

/**
 * Represents a directory message (server -> clients)
 */
class Directory : public Message {
   public:
    std::vector<Address*> client_addrs_;

    /**
     * Creates instance of a directory message.
     * @arg addrs  the addresss of the clients that are registered
     */
    Directory(std::unordered_map<size_t, Address*>& addrs)
        : Message(MsgType::DIRECTORY), client_addrs_() {
        for (size_t i = 0; i < addrs.size(); i++) {
            client_addrs_.push_back(addrs[i]);
        }
    }

    Directory(Deserializer* d) : Message(MsgType::DIRECTORY, d), client_addrs_() {
        size_t num_addrs = d->get_size_t();
        for (size_t i = 0; i < num_addrs; i++) {
            client_addrs_.push_back(new Address(d));
        }
    }

    /**
     * Deconstructs an instance of a register message.
     */
    virtual ~Directory() {}

    /**
     * Serializes the object into a string of chars.
     */
    virtual void serialize(Serializer* s) {
        Message::serialize(s);
        s->add_size_t(client_addrs_.size());
        for (Address* addr : client_addrs_) {
            addr->serialize(s);
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
            client_addrs_ == o->client_addrs_) {
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
