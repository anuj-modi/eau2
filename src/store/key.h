#pragma once
#include "util/object.h"
#include "util/serial.h"
#include "util/string.h"
#include <string>

/**
 * Array: Represents a key in a key value store.
 * Author: gomes.chri, modi.an
 */
class Key : public Object {
   public:
    std::string k_;
    size_t node_;

    Key() : Key("", 0) {}

    Key(const char* k, size_t node) : Object() {
        k_ = std::string(k);
        node_ = node;
    }

    Key(const char* k) : Key(k, 0) {}

    Key(Deserializer* d) : Object() {
        String* temp = d->get_string();
        k_ = std::string(temp->c_str());
        node_ = d->get_size_t();
        delete temp;
    }

    Key(const Key& k) : Object() {
        node_ = k.node_;
        k_ = std::string(k.k_);
    }

    virtual ~Key() {
    }

    /**
     * Sets the node number.
     * @arg n  the node number
     */
    void set_node(size_t n) {
        node_ = n;
    }

    /**
     * Checks if this key is equal to another object.
     * Keys are equal if their String values are the same
     * but they have different node numbers.
     * @arg other  the other object
     * @return if the two are equal
     */
    bool equals(Object* other) {
        if (other == this) return true;
        Key* o = dynamic_cast<Key*>(other);
        if (o == nullptr) return false;
        return k_ == o->k_;
    }

    /** Compute a hash for this key. */
    size_t hash_me() {
        return std::hash<std::string>()(k_);
    }

    /**
     * Makes a copy of the key.
     * @return the copy
     */
    Key* clone() {
        return new Key(*this);
    }

    /**
     * Serializes the key.
     * @arg s  the serializer
     */
    void serialize(Serializer* s) {
        String* temp = new String(k_.c_str());
        s->add_string(temp);
        s->add_size_t(node_);
        delete temp;
    }

    bool operator==(const Key& k) const {
        return k.k_ == k_;
    }
};

namespace std {
template <>
struct hash<Key> {
    size_t operator()(const Key& k) const {
        return hash<string>()(k.k_);
    }
};
}  // namespace std
