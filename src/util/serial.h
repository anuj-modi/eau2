#pragma once
#include <stdio.h>
#include "object.h"
#include "string.h"

enum class MsgType;

class Serializer : public Object {
   public:
    char* bytes_;
    size_t size_;
    size_t capacity_;

    Serializer() : Object() {
        size_ = 0;
        capacity_ = 8;
        bytes_ = new char[capacity_];
    }

    virtual ~Serializer() {
        delete[] bytes_;
    }

    char* get_bytes() {
        return bytes_;
    }

    size_t size() {
        return size_;
    }

    /** Private helper function which expands the array
     * @arg min_capacity  the min length that the array has to be
     */
    void expand_(size_t min_capacity) {
        // figure out how big we should expand our list
        for (size_t factor = 2;; factor++) {
            // if this factor can the desired size
            if (min_capacity < capacity_ * factor) {
                // allocate a larer array and copy items over
                char* new_bytes = new char[capacity_ * factor];
                for (size_t i = 0; i < size_; i++) {
                    new_bytes[i] = bytes_[i];
                }
                // deallocate the old array
                delete[] bytes_;

                // update to the new array
                bytes_ = new_bytes;
                capacity_ *= factor;
                return;
            }
        }
    }

    void add_int(int val) {
        size_t new_size = size_ + sizeof(val);
        if (new_size > capacity_) {
            expand_(new_size);
        }

        int* ptr = (int*)(bytes_ + size_);
        *ptr = val;
        size_ = new_size;
    }

    void add_double(double val) {
        size_t new_size = size_ + sizeof(val);
        if (new_size > capacity_) {
            expand_(new_size);
        }

        double* ptr = (double*)(bytes_ + size_);
        *ptr = val;
        size_ = new_size;
    }

    void add_bool(bool val) {
        size_t new_size = size_ + sizeof(val);
        if (new_size > capacity_) {
            expand_(new_size);
        }

        bool* ptr = (bool*)(bytes_ + size_);
        *ptr = val;
        size_ = new_size;
    }

    void add_uint32_t(uint32_t val) {
        size_t new_size = size_ + sizeof(val);
        if (new_size > capacity_) {
            expand_(new_size);
        }

        uint32_t* ptr = (uint32_t*)(bytes_ + size_);
        *ptr = val;
        size_ = new_size;
    }

    void add_size_t(size_t val) {
        size_t new_size = size_ + sizeof(val);
        if (new_size > capacity_) {
            expand_(new_size);
        }

        size_t* ptr = (size_t*)(bytes_ + size_);
        *ptr = val;
        size_ = new_size;
    }

    void add_msg_type(MsgType val) {
        size_t new_size = size_ + sizeof(val);
        if (new_size > capacity_) {
            expand_(new_size);
        }

        MsgType* ptr = (MsgType*)(bytes_ + size_);
        *ptr = val;
        size_ = new_size;
    }

    void add_buffer(const void* buf, size_t num_bytes) {
        assert(buf != nullptr);
        size_t new_size = size_ + num_bytes;
        if (new_size > capacity_) {
            expand_(new_size);
        }

        memcpy(bytes_ + size_, buf, num_bytes);
        size_ = new_size;
    }

    void add_string(String* s) {
        assert(s != nullptr);
        add_size_t(s->size());
        add_buffer(s->c_str(), s->size());
    }
};

class Deserializer : public Object {
   public:
    char* bytes_;
    char* current_;
    size_t bytes_remaining_;

    Deserializer(const char* buf, size_t num_bytes) : Object() {
        bytes_ = new char[num_bytes];
        memcpy(bytes_, buf, num_bytes);
        current_ = bytes_;
        bytes_remaining_ = num_bytes;
    }

    virtual ~Deserializer() {
        delete[] bytes_;
    }

    int get_int() {
        assert(bytes_remaining_ >= sizeof(int));
        int result = *((int*)current_);
        current_ += sizeof(int);
        bytes_remaining_ -= sizeof(int);
        return result;
    }

    double get_double() {
        assert(bytes_remaining_ >= sizeof(double));
        double result = *((double*)current_);
        current_ += sizeof(double);
        bytes_remaining_ -= sizeof(double);
        return result;
    }

    bool get_bool() {
        assert(bytes_remaining_ >= sizeof(bool));
        bool result = *((bool*)current_);
        current_ += sizeof(bool);
        bytes_remaining_ -= sizeof(bool);
        return result;
    }

    uint32_t get_uint32_t() {
        assert(bytes_remaining_ >= sizeof(uint32_t));
        uint32_t result = *((uint32_t*)current_);
        current_ += sizeof(uint32_t);
        bytes_remaining_ -= sizeof(uint32_t);
        return result;
    }

    size_t get_size_t() {
        assert(bytes_remaining_ >= sizeof(size_t));
        size_t result = *((size_t*)current_);
        current_ += sizeof(size_t);
        bytes_remaining_ -= sizeof(size_t);
        return result;
    }

    MsgType get_msg_type() {
        assert(bytes_remaining_ >= sizeof(MsgType));
        MsgType result = *((MsgType*)current_);
        current_ += sizeof(MsgType);
        bytes_remaining_ -= sizeof(MsgType);
        return result;
    }

    char* get_buffer(size_t num_bytes) {
        assert(bytes_remaining_ >= num_bytes);
        char* result = new char[num_bytes];
        get_buffer(num_bytes, result);
        return result;
    }

    void get_buffer(size_t num_bytes, char* buf) {
        assert(bytes_remaining_ >= num_bytes);
        memcpy(buf, current_, num_bytes);
        current_ += num_bytes;
        bytes_remaining_ -= num_bytes;
    }

    String* get_string() {
        size_t len = get_size_t();
        char* c_str = new char[len + 1];
        get_buffer(len, c_str);
        c_str[len] = '\0';
        return new String(true, c_str, len);
    }
};
