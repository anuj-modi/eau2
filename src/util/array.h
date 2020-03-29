// lang:CwC
#pragma once
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "object.h"
#include "serial.h"
#include "string.h"

union Payload {
    int i;
    double d;
    bool b;
    String* s;
};

class Data {
   public:
    bool missing;
    Payload payload;
};

/**
 * Array: Represents a array.
 * Author: gomes.chri, modi.an
 */
class Array : public Object {
   public:
    size_t size_;
    size_t capacity_;
    Data* items_;

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    Array(size_t max_size) : Object() {
        size_ = 0;
        capacity_ = max_size;
        items_ = new Data[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    virtual ~Array() {
        delete[] items_;
    }

    /**
     * Adds an element to the end the array.
     * @arg o  element to add
     */
    virtual void push_back(Data d) {
        assert(size_ < capacity_);
        items_[size_] = d;
        size_ += 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual Data get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Gets the number of elements in the array.
     * @return the number of elements
     */
    virtual size_t size() {
        return size_;
    }

    /**
     * Serializes the array.
     * arg s  the serializer to use
     */
    virtual void serialize(Serializer* s) {
        assert(false);
    }
};

/**
 * Array: Represents an integer array.
 * Author: gomes.chri, modi.an
 */
class IntArray : public Array {
    public:
    IntArray(size_t max_size) : Array(max_size) {}

    /**
     * Adds an element to the end the array.
     * @arg i  element to add
     */
    virtual void push_back(int i) {
        Data d;
        d.missing = false;
        d.payload.i = i;
        Array::push_back(d);
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual int get_int(size_t i) {
        return Array::get(i).payload.i;
    }

    /**
     * Serializes the array.
     * arg s  the serializer to use
     */
    virtual void serialize(Serializer* s) {
        s->add_size_t(size_);
        for(size_t i = 0; i < size_; i++) {
            s->add_int(get_int(i));
        }
    }
};

/**
 * Array: Represents an double array.
 * Author: gomes.chri, modi.an
 */
class DoubleArray : public Array {
    public:
    DoubleArray(size_t max_size) : Array(max_size) {}

    /**
     * Adds an element to the end the array.
     * @arg i  element to add
     */
    virtual void push_back(double v) {
        Data d;
        d.missing = false;
        d.payload.d = v;
        Array::push_back(d);
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual double get_double(size_t i) {
        return Array::get(i).payload.d;
    }

    /**
     * Serializes the array.
     * arg s  the serializer to use
     */
    virtual void serialize(Serializer* s) {
        s->add_size_t(size_);
        for(size_t i = 0; i < size_; i++) {
            s->add_double(get_double(i));
        }
    }
};

/**
 * Array: Represents an boolean array.
 * Author: gomes.chri, modi.an
 */
class BoolArray : public Array {
    public:
    BoolArray(size_t max_size) : Array(max_size) {}

    /**
     * Adds an element to the end the array.
     * @arg b  element to add
     */
    virtual void push_back(bool b) {
        Data d;
        d.missing = false;
        d.payload.b = b;
        Array::push_back(d);
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual bool get_bool(size_t i) {
        return Array::get(i).payload.b;
    }

    /**
     * Serializes the array.
     * arg s  the serializer to use
     */
    virtual void serialize(Serializer* s) {
        s->add_size_t(size_);
        for(size_t i = 0; i < size_; i++) {
            s->add_bool(get_bool(i));
        }
    }
};

/**
 * Array: Represents an String array.
 * Author: gomes.chri, modi.an
 */
class StringArray : public Array {
    public:
    StringArray(size_t max_size) : Array(max_size) {}

    /**
     * Adds an element to the end the array.
     * @arg s  element to add
     */
    virtual void push_back(String* s) {
        Data d;
        d.missing = false;
        d.payload.s = s;
        Array::push_back(d);
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual String* get_string(size_t i) {
        return Array::get(i).payload.s;
    }

    /**
     * Deletes the strings in the array.
     */
    void delete_items() {
        for (size_t i = 0; i < size(); i++) {
            delete get(i).payload.s;
        }
    }

    /**
     * Serializes the array.
     * arg s  the serializer to use
     */
    virtual void serialize(Serializer* s) {
        s->add_size_t(size_);
        for(size_t i = 0; i < size_; i++) {
            s->add_string(get_string(i));
        }
    }
};