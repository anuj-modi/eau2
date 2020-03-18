#pragma once
#include "util/object.h"
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

    virtual ~Key() {
        delete k_;
    }

    /**
     * Checks if this key is equal to another object.
     * @arg other  the other object
     * @return if the two are equal
     */
    bool equals(Object* other) {
        if (other == this) return true;
        Key* o = dynamic_cast<Key*>(other);
        if (o == nullptr) return false;
        return k_->equals(o->k_) && node_ == o->node_;
    }

    /** Compute a hash for this key. */
    size_t hash_me() {
        return k_->hash() * (node_ + 1);
    }
};
