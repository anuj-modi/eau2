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
    size_t size_;

    Value(char* blob, size_t size) : Value(false, blob, size) {}

    Value(bool steal, char* blob, size_t size) : Object() {
        if (steal) {
            blob_ = blob;
            size_ = size;
        } else {
            blob_ = new char[size];
            memcpy(blob_, blob, size);
            size_ = size;
        }
    }

    virtual ~Value() {
        delete[] blob_;
    }

    /**
     * Gets the data stored in the value object.
     * @return the data as bytes
     */
    char* get_bytes() {
        return blob_;
    }

    /**
     * Gets the size of the data stored in the value object.
     * @return the number of bytes
     */
    size_t get_size() {
        return size_;
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
        return o->size_ == size_ && memcmp(blob_, o->blob_, size_) == 0;
    }

    /** Compute a hash for this key. */
    size_t hash_me() {
        String str_blob(blob_, size_);
        return str_blob.hash();
    }
};
