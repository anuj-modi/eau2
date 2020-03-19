#pragma once
#include "util/object.h"
#include "util/string.h"

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
        Value* o = dynamic_cast<Value*>(other);
        if (o == nullptr) return false;
        String* this_blob = new String(blob_);
        String* other_blob = new String(o->blob_);
        return this_blob->equals(other_blob);
    }

    /** Compute a hash for this key. */
    size_t hash_me() {
        String str_blob(blob_);
        return str_blob.hash();
    }
};
