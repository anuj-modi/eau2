#pragma once
#include "util/object.h"
#include "util/serial.h"
#include "util/string.h"

/**
 * Array: Represents a key in a key value store.
 * Author: gomes.chri, modi.an
 */
class Key : public Object {
   public:
    String* k_;
    size_t node_;

    Key(const char* k, size_t node) : Object() {
        k_ = new String(k);
        node_ = node;
    }

    Key(const char* k) : Key(k, 0) {}

    Key(Deserializer* d) : Object() {
        k_ = d->get_string();
        node_ = d->get_size_t();
    }

    virtual ~Key() {
        delete k_;
    }

    // TODO test key set_node
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
        return k_->equals(o->k_);
    }

    /** Compute a hash for this key. */
    size_t hash_me() {
        return k_->hash();
    }

    // TODO test key clone
    /**
     * Makes a copy of the key.
     * @return the copy
     */
    Key* clone() {
        return new Key(k_->c_str(), node_);
    }

    /**
     * Serializes the key.
     * @arg s  the serializer
     */
    void serialize(Serializer* s) {
        s->add_string(k_);
        s->add_size_t(node_);
    }
};

namespace std {
template <>
struct hash<Key> {
    size_t operator()(const Key& k) const {
        // Compute individual hash values for two data members and combine them using XOR and bit
        // shifting
        return (k.k_->hash());
    }
};
}  // namespace std
