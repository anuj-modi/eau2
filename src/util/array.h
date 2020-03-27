// lang:CwC
#pragma once
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "object.h"
#include "serial.h"
#include "string.h"

/**
 * Array: Represents a resizeable array.
 * Author: gomes.chri, modi.an
 */
class Array : public Object {
   public:
    size_t size_;
    size_t capacity_;
    Object** items_;

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    Array() : Object() {
        size_ = 0;
        capacity_ = 10;
        items_ = new Object*[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    virtual ~Array() {
        delete[] items_;
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
                Object** new_items = new Object*[capacity_ * factor];
                for (size_t i = 0; i < size_; i++) {
                    new_items[i] = items_[i];
                }
                // deallocate the old array
                delete[] items_;

                // update to the new array
                items_ = new_items;
                capacity_ *= factor;
                return;
            }
        }
    }

    /**
     * Adds an element to the end the array.
     * @arg o  element to add
     */
    virtual void push_back(Object* o) {
        // if our array is full
        if (size_ == capacity_) {
            expand_(size_ + 1);
        }
        items_[size_] = o;
        size_ += 1;
    }

    /**
     * Concats another array onto the end of this array.
     * @arg arr  array to concat
     */
    virtual void push_back_all(Array* arr) {
        for (size_t i = 0; i < arr->size(); i++) {
            push_back(arr->get(i));
        }
    }

    /**
     * Adds an item at a specific index in the Array
     * @arg i  position to add the new item
     * @arg o  the new item
     */
    virtual void add(size_t i, Object* o) {
        assert(i <= size_);
        if (size_ == i) {
            push_back(o);
        } else {
            if (size_ == capacity_) {
                expand_(size_ + 1);
            }
            for (size_t k = size_; k > i; k--) {
                items_[k] = items_[k - 1];
            }
            items_[i] = o;
            size_ += 1;
        }
    }

    /**
     * Adds all items from given array at a specific index in the array
     * @arg i  position to add new items
     * @arg a  array of items to add
     */
    virtual void add_all(size_t i, Array* a) {
        assert(i <= size_);
        if (a == nullptr || a->size() == 0) {
            return;
        }
        if (i == size_) {
            push_back_all(a);
            return;
        }
        size_t new_size = a->size_ + size_;

        // If we can't fit both lists in current list
        if (new_size > capacity_) {
            expand_(new_size);
        }
        size_t second_start = i + a->size();
        size_t curr_loc = i;
        for (size_t m = second_start; m < new_size; m++) {
            items_[m] = items_[curr_loc];
            curr_loc++;
        }
        size_t c_loc = 0;
        for (size_t k = i; k < second_start; k++) {
            items_[k] = a->get(c_loc);
            c_loc++;
        }
        size_ = new_size;
    }

    /**
     * Removes all elements from the array.
     */
    virtual void clear() {
        size_ = 0;
        capacity_ = 10;
        delete[] items_;
        items_ = new Object*[capacity_];
    }

    /**
     * Checks if the item exists in the array.
     * @arg o  element to find in the array
     * @return if item in array
     */
    virtual bool contains(Object* o) {
        if (o == nullptr) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (o->equals(items_[i])) {
                return true;
            }
        }
        return false;
    }

    /**
     * Gets the index of the element given. If not in list,
     * returns size + 1.
     * @arg o  element to find in the list
     * @return index of the element if found
     */
    virtual size_t index_of(Object* o) {
        // return size + 1 if it isn't
        if (o == nullptr) {
            return size_ + 1;
        }

        // look for the provided string in the list
        for (size_t i = 0; i < size_; i++) {
            // If i found one which is equal, return the index
            if (items_[i]->equals(o)) {
                return i;
            }
        }

        // couldn't find the string, return size
        return size_ + 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual Object* get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Checks if the array is empty.
     * @return if the array is empty
     */
    virtual bool is_empty() {
        return size_ == 0;
    }

    /**
     * Gets the number of elements in the array.
     * @return the number of elements
     */
    virtual size_t size() {
        return size_;
    }

    /**
     * Removes the element at the given index.
     * @arg i  index to remove from
     * @return removed element
     */
    virtual Object* remove(size_t i) {
        assert(i < size_);
        Object* o = items_[i];
        for (size_t j = i; j < size_ - 1; j++) {
            items_[j] = items_[j + 1];
        }
        --size_;
        return o;
    }

    /**
     * Replaces the element at the index with given one.
     * @arg i  index to replace the element at
     * @arg e  element to replace with
     * @return replaced element
     */
    virtual Object* set(size_t i, Object* e) {
        assert(i < size_);
        Object* o = items_[i];
        items_[i] = e;
        return o;
    }

    /**
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Array* o = dynamic_cast<Array*>(other);
        if (o == nullptr || size_ != o->size_) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (!items_[i]->equals(o->get(i))) {
                return false;
            }
        }
        return true;
    }

    /**
     * Hashes the array.
     * @return the hashed value
     */
    virtual size_t hash() {
        size_t hash = 0;
        for (size_t i = 0; i < size_; i++) {
            hash += items_[i]->hash() * (i + 1);
        }
        return hash;
    }

    /**
     * Gets a copy of the array.
     * @return the copy of the array
     */
    virtual Array* clone() {
        Array* items_copy = new Array();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }
};

/**
 * Array: Represents an int resizeable array.
 * Author: gomes.chri, modi.an
 */
class IntArray : public Object {
   public:
    size_t size_;
    size_t capacity_;
    int* items_;

    /**
     * Constructs a DoubleArray from a Deserializer.
     */
    IntArray(Deserializer* d) : Object() {
        size_ = d->get_size_t();
        capacity_ = size_;
        items_ = (int*)d->get_buffer(size_ * sizeof(int));
    }

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    IntArray() : Object() {
        size_ = 0;
        capacity_ = 10;
        items_ = new int[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    virtual ~IntArray() {
        delete[] items_;
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
                int* new_items = new int[capacity_ * factor];
                for (size_t i = 0; i < size_; i++) {
                    new_items[i] = items_[i];
                }
                // deallocate the old array
                delete[] items_;

                // update to the new array
                items_ = new_items;
                capacity_ *= factor;
                return;
            }
        }
    }

    /**
     * Adds an element to the end the array.
     * @arg o  element to add
     */
    void push_back(int o) {
        // if our array is full
        if (size_ == capacity_) {
            expand_(size_ + 1);
        }
        items_[size_] = o;
        size_ += 1;
    }

    /**
     * Concats another array onto the end of this array.
     * @arg arr  array to concat
     */
    void push_back_all(IntArray* arr) {
        for (size_t i = 0; i < arr->size(); i++) {
            push_back(arr->get(i));
        }
    }

    /**
     * Adds an item at a specific index in the Array
     * @arg i  position to add the new item
     * @arg o  the new item
     */
    void add(size_t i, int o) {
        assert(i <= size_);
        if (size_ == i) {
            push_back(o);
        } else {
            if (size_ == capacity_) {
                expand_(size_ + 1);
            }
            for (size_t k = size_; k > i; k--) {
                items_[k] = items_[k - 1];
            }
            items_[i] = o;
            size_ += 1;
        }
    }

    /**
     * Adds all items from given array at a specific index in the array
     * @arg i  position to add new items
     * @arg a  array of items to add
     */
    void add_all(size_t i, IntArray* a) {
        assert(i <= size_);
        if (a == nullptr || a->size() == 0) {
            return;
        }
        if (i == size_) {
            push_back_all(a);
            return;
        }
        size_t new_size = a->size_ + size_;

        // If we can't fit both lists in current list
        if (new_size > capacity_) {
            expand_(new_size);
        }
        size_t second_start = i + a->size();
        size_t curr_loc = i;
        for (size_t m = second_start; m < new_size; m++) {
            items_[m] = items_[curr_loc];
            curr_loc++;
        }
        size_t c_loc = 0;
        for (size_t k = i; k < second_start; k++) {
            items_[k] = a->get(c_loc);
            c_loc++;
        }
        size_ = new_size;
    }

    /**
     * Removes all elements from the array.
     */
    void clear() {
        size_ = 0;
        capacity_ = 10;
        delete[] items_;
        items_ = new int[capacity_];
    }

    /**
     * Checks if the item exists in the array.
     * @arg o  element to find in the array
     * @return if item in array
     */
    bool contains(int o) {
        for (size_t i = 0; i < size_; i++) {
            if (o == items_[i]) {
                return true;
            }
        }
        return false;
    }

    /**
     * Gets the index of the element given. If not in list,
     * returns size + 1.
     * @arg o  element to find in the list
     * @return index of the element if found
     */
    size_t index_of(int o) {
        // look for the provided string in the list
        for (size_t i = 0; i < size_; i++) {
            // If i found one which is equal, return the index
            if (items_[i] == o) {
                return i;
            }
        }

        // couldn't find the string, return size + 1
        return size_ + 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    int get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Checks if the array is empty.
     * @return if the array is empty
     */
    bool is_empty() {
        return size_ == 0;
    }

    /**
     * Gets the number of elements in the array.
     * @return the number of elements
     */
    size_t size() {
        return size_;
    }

    /**
     * Removes the element at the given index.
     * @arg i  index to remove from
     * @return removed element
     */
    int remove(size_t i) {
        assert(i < size_);
        int o = items_[i];
        for (size_t j = i; j < size_ - 1; j++) {
            items_[j] = items_[j + 1];
        }
        --size_;
        return o;
    }

    /**
     * Replaces the element at the index with given one.
     * @arg i  index to replace the element at
     * @arg e  element to replace with
     * @return replaced element
     */
    int set(size_t i, int e) {
        assert(i < size_);
        int o = items_[i];
        items_[i] = e;
        return o;
    }

    /**
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        IntArray* o = dynamic_cast<IntArray*>(other);
        if (o == nullptr || size_ != o->size_) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (items_[i] != o->get(i)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Hashes the array.
     * @return the hashed value
     */
    virtual size_t hash() {
        size_t hash = 0;
        for (size_t i = 0; i < size_; i++) {
            hash += items_[i] * (i + 1);
        }
        return hash;
    }

    /**
     * Gets a copy of the array.
     * @return the copy of the array
     */
    IntArray* clone() {
        IntArray* items_copy = new IntArray();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }

    /**
     * Serializes this array.
     * @arg s  the serializer to add the fields to
     */
    void serialize(Serializer* s) {
        s->add_size_t(size_);
        s->add_buffer(items_, size_ * sizeof(int));
    }
};

class StringArray : public Array {
   public:
    StringArray() : Array() {}

    StringArray(Deserializer* d) : StringArray() {
        size_t num_strs = d->get_size_t();
        for (size_t i = 0; i < num_strs; i++) {
            push_back(d->get_string());
        }
    }

    virtual ~StringArray() {}

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return string at the index
     */
    virtual String* get(size_t i) {
        return static_cast<String*>(Array::get(i));
    }

    /**
     * Removes the element at the given index.
     * @arg i  index to remove from
     * @return removed element
     */
    virtual String* remove(size_t i) {
        return static_cast<String*>(Array::remove(i));
    }

    /**
     * Replaces the string at the index with given one.
     * @arg i  index to replace the element at
     * @arg s  string to replace with
     * @return replaced string
     */
    virtual String* set(size_t i, String* s) {
        return static_cast<String*>(Array::set(i, s));
    }

    /**
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        return Array::equals(other);
    }

    /**
     * Hashes the array.
     * @return the hashed value
     */
    virtual size_t hash() {
        return Array::hash();
    }

    /**
     * Gets a copy of the array.
     * @return the copy of the array
     */
    virtual StringArray* clone() {
        StringArray* items_copy = new StringArray();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }

    void serialize(Serializer* s) {
        s->add_size_t(size());
        for (size_t i = 0; i < size(); i++) {
            s->add_string(get(i));
        }
    }
};

/**
 * Array: Represents an bool resizeable array.
 * Author: gomes.chri, modi.an
 */
class BoolArray : public Object {
   public:
    size_t size_;
    size_t capacity_;
    bool* items_;

    /**
     * Constructs a DoubleArray from a Deserializer.
     */
    BoolArray(Deserializer* d) {
        size_ = d->get_size_t();
        capacity_ = size_;
        items_ = (bool*)d->get_buffer(size_ * sizeof(bool));
    }

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    BoolArray() : Object() {
        size_ = 0;
        capacity_ = 10;
        items_ = new bool[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    ~BoolArray() {
        delete[] items_;
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
                bool* new_items = new bool[capacity_ * factor];
                for (size_t i = 0; i < size_; i++) {
                    new_items[i] = items_[i];
                }
                // deallocate the old array
                delete[] items_;

                // update to the new array
                items_ = new_items;
                capacity_ *= factor;
                return;
            }
        }
    }

    /**
     * Adds an element to the end the array.
     * @arg o  element to add
     */
    virtual void push_back(bool o) {
        // if our array is full
        if (size_ == capacity_) {
            expand_(size_ + 1);
        }
        items_[size_] = o;
        size_ += 1;
    }

    /**
     * Concats another array onto the end of this array.
     * @arg arr  array to concat
     */
    virtual void push_back_all(BoolArray* arr) {
        for (size_t i = 0; i < arr->size(); i++) {
            push_back(arr->get(i));
        }
    }

    /**
     * Adds an item at a specific index in the Array
     * @arg i  position to add the new item
     * @arg o  the new item
     */
    virtual void add(size_t i, bool o) {
        assert(i <= size_);
        if (size_ == i) {
            push_back(o);
        } else {
            if (size_ == capacity_) {
                expand_(size_ + 1);
            }
            for (size_t k = size_; k > i; k--) {
                items_[k] = items_[k - 1];
            }
            items_[i] = o;
            size_ += 1;
        }
    }

    /**
     * Adds all items from given array at a specific index in the array
     * @arg i  position to add new items
     * @arg a  array of items to add
     */
    virtual void add_all(size_t i, BoolArray* a) {
        assert(i <= size_);
        if (a == nullptr || a->size() == 0) {
            return;
        }
        if (i == size_) {
            push_back_all(a);
            return;
        }
        size_t new_size = a->size_ + size_;

        // If we can't fit both lists in current list
        if (new_size > capacity_) {
            expand_(new_size);
        }
        size_t second_start = i + a->size();
        size_t curr_loc = i;
        for (size_t m = second_start; m < new_size; m++) {
            items_[m] = items_[curr_loc];
            curr_loc++;
        }
        size_t c_loc = 0;
        for (size_t k = i; k < second_start; k++) {
            items_[k] = a->get(c_loc);
            c_loc++;
        }
        size_ = new_size;
    }

    /**
     * Removes all elements from the array.
     */
    virtual void clear() {
        size_ = 0;
        capacity_ = 10;
        delete[] items_;
        items_ = new bool[capacity_];
    }

    /**
     * Checks if the item exists in the array.
     * @arg o  element to find in the array
     * @return if item in array
     */
    virtual bool contains(bool o) {
        for (size_t i = 0; i < size_; i++) {
            if (o == items_[i]) {
                return true;
            }
        }
        return false;
    }

    /**
     * Gets the index of the first occurance of the element given. If not in list,
     * returns size + 1.
     * @arg o  element to find in the list
     * @return index of the element if found
     */
    virtual size_t index_of(bool o) {
        // look for the provided string in the list
        for (size_t i = 0; i < size_; i++) {
            // If i found one which is equal, return the index
            if (items_[i] == o) {
                return i;
            }
        }

        // couldn't find the string, return size + 1
        return size_ + 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual bool get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Checks if the array is empty.
     * @return if the array is empty
     */
    virtual bool is_empty() {
        return size_ == 0;
    }

    /**
     * Gets the number of elements in the array.
     * @return the number of elements
     */
    virtual size_t size() {
        return size_;
    }

    /**
     * Removes the element at the given index.
     * @arg i  index to remove from
     * @return removed element
     */
    virtual bool remove(size_t i) {
        assert(i < size_);
        bool o = items_[i];
        for (size_t j = i; j < size_ - 1; j++) {
            items_[j] = items_[j + 1];
        }
        --size_;
        return o;
    }

    /**
     * Replaces the element at the index with given one.
     * @arg i  index to replace the element at
     * @arg e  element to replace with
     * @return replaced element
     */
    virtual bool set(size_t i, bool e) {
        assert(i < size_);
        bool o = items_[i];
        items_[i] = e;
        return o;
    }

    /**
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        BoolArray* o = dynamic_cast<BoolArray*>(other);
        if (o == nullptr || size_ != o->size_) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (!items_[i] == o->get(i)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Hashes the array.
     * @return the hashed value
     */
    virtual size_t hash() {
        size_t hash = 0;
        for (size_t i = 0; i < size_; i++) {
            hash += items_[i] * (i + 1);
        }
        return hash;
    }

    /**
     * Gets a copy of the array.
     * @return the copy of the array
     */
    virtual BoolArray* clone() {
        BoolArray* items_copy = new BoolArray();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }

    /**
     * Serializes this array.
     * @arg s  the serializer to add the fields to
     */
    void serialize(Serializer* s) {
        s->add_size_t(size_);
        s->add_buffer(items_, size_ * sizeof(bool));
    }
};

/**
 * Array: Represents an double resizeable array.
 * Author: gomes.chri, modi.an
 */
class DoubleArray : public Object {
   public:
    size_t size_;
    size_t capacity_;
    double* items_;

    /**
     * Constructs a DoubleArray from a Deserializer.
     */
    DoubleArray(Deserializer* d) {
        size_ = d->get_size_t();
        capacity_ = size_;
        items_ = (double*)d->get_buffer(size_ * sizeof(double));
    }

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    DoubleArray() : Object() {
        size_ = 0;
        capacity_ = 10;
        items_ = new double[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    ~DoubleArray() {
        delete[] items_;
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
                double* new_items = new double[capacity_ * factor];
                for (size_t i = 0; i < size_; i++) {
                    new_items[i] = items_[i];
                }
                // deallocate the old array
                delete[] items_;

                // update to the new array
                items_ = new_items;
                capacity_ *= factor;
                return;
            }
        }
    }

    /**
     * Adds an element to the end the array.
     * @arg o  element to add
     */
    virtual void push_back(double o) {
        // if our array is full
        if (size_ == capacity_) {
            expand_(size_ + 1);
        }
        items_[size_] = o;
        size_ += 1;
    }

    /**
     * Concats another array onto the end of this array.
     * @arg arr  array to concat
     */
    virtual void push_back_all(DoubleArray* arr) {
        for (size_t i = 0; i < arr->size(); i++) {
            push_back(arr->get(i));
        }
    }

    /**
     * Adds an item at a specific index in the Array
     * @arg i  position to add the new item
     * @arg o  the new item
     */
    virtual void add(size_t i, double o) {
        assert(i <= size_);
        if (size_ == i) {
            push_back(o);
        } else {
            if (size_ == capacity_) {
                expand_(size_ + 1);
            }
            for (size_t k = size_; k > i; k--) {
                items_[k] = items_[k - 1];
            }
            items_[i] = o;
            size_ += 1;
        }
    }

    /**
     * Adds all items from given array at a specific index in the array
     * @arg i  position to add new items
     * @arg a  array of items to add
     */
    virtual void add_all(size_t i, DoubleArray* a) {
        assert(i <= size_);
        if (a == nullptr || a->size() == 0) {
            return;
        }
        if (i == size_) {
            push_back_all(a);
            return;
        }
        size_t new_size = a->size_ + size_;

        // If we can't fit both lists in current list
        if (new_size > capacity_) {
            expand_(new_size);
        }
        size_t second_start = i + a->size();
        size_t curr_loc = i;
        for (size_t m = second_start; m < new_size; m++) {
            items_[m] = items_[curr_loc];
            curr_loc++;
        }
        size_t c_loc = 0;
        for (size_t k = i; k < second_start; k++) {
            items_[k] = a->get(c_loc);
            c_loc++;
        }
        size_ = new_size;
    }

    /**
     * Removes all elements from the array.
     */
    virtual void clear() {
        size_ = 0;
        capacity_ = 10;
        delete[] items_;
        items_ = new double[capacity_];
    }

    /**
     * Checks if the item exists in the array.
     * @arg o  element to find in the array
     * @return if item in array
     */
    virtual bool contains(double o) {
        for (size_t i = 0; i < size_; i++) {
            if (fabs(o - items_[i]) < 0.0000001) {
                return true;
            }
        }
        return false;
    }

    /**
     * Gets the index of the element given. If not in list,
     * returns size + 1.
     * @arg o  element to find in the list
     * @return index of the element if found
     */
    virtual size_t index_of(double o) {
        // look for the provided string in the list
        for (size_t i = 0; i < size_; i++) {
            // If i found one which is equal, return the index
            if (fabs(o - items_[i]) < 0.0000001) {
                return i;
            }
        }

        // couldn't find the string, return size + 1
        return size_ + 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual double get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Checks if the array is empty.
     * @return if the array is empty
     */
    virtual bool is_empty() {
        return size_ == 0;
    }

    /**
     * Gets the number of elements in the array.
     * @return the number of elements
     */
    virtual size_t size() {
        return size_;
    }

    /**
     * Removes the element at the given index.
     * @arg i  index to remove from
     * @return removed element
     */
    virtual double remove(size_t i) {
        assert(i < size_);
        double o = items_[i];
        for (size_t j = i; j < size_ - 1; j++) {
            items_[j] = items_[j + 1];
        }
        --size_;
        return o;
    }

    /**
     * Replaces the element at the index with given one.
     * @arg i  index to replace the element at
     * @arg e  element to replace with
     * @return replaced element
     */
    virtual double set(size_t i, double e) {
        assert(i < size_);
        double o = items_[i];
        items_[i] = e;
        return o;
    }

    /**
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        DoubleArray* o = dynamic_cast<DoubleArray*>(other);
        if (o == nullptr || size_ != o->size_) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (fabs(o->get(i) - items_[i]) > 0.0000001) {
                return false;
            }
        }
        return true;
    }

    /**
     * Hashes the array.
     * @return the hashed value
     */
    virtual size_t hash() {
        size_t hash = 0;
        for (size_t i = 0; i < size_; i++) {
            hash += items_[i] * (i + 1);
        }
        return hash;
    }

    /**
     * Gets a copy of the array.
     * @return the copy of the array
     */
    virtual DoubleArray* clone() {
        DoubleArray* items_copy = new DoubleArray();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }

    /**
     * Serializes this array.
     * @arg s  the serializer to add the fields to
     */
    void serialize(Serializer* s) {
        s->add_size_t(size_);
        s->add_buffer(items_, size_ * sizeof(double));
    }
};
