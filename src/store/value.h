#pragma once
#include "~/util/object.h"
#include "~/util/string.h"

/**
 * Array: Represents a value in a key value store.
 * Author: gomes.chri, modi.an
 */
class Value : public Object {
   public:
    char* blob_;

    Value(char* blob) : Object() {
        blob_ = blob;
    }

    virtual ~Value() {}

    /**
     * Gets the data stored in the value object.
     * @return the data as bytes
     */
    char* get_bytes() {
        return blob_;
    }

    /**
     * Checks if this key is equal to another object.
     * @arg other  the other object
     * @return if the two are equal
     */
    bool equals(Object* other) {
        if (other == this) return true;
        Key* o = dynamic_cast<Key*>(other);
        if (x == nullptr) return false;
        return k_->equals(o->k_) && node_ == o->node_;
    }

    /** Compute a hash for this key. */
    size_t hash_me() {
        return k_->hash() * (node_ + 1);
    }
}