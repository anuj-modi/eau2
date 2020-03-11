#pragma once
#include <assert.h>
#include "helper.h"
#include "object.h"
// LANGUAGE: CwC

template <typename T>
class Array : public Object {
   public:
    size_t size_;
    size_t capacity_;
    T* items_;

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    Array() : Object() {
        size_ = 0;
        capacity_ = 10;
        items_ = new T[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    ~Array() {
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
                T* new_items = new T[capacity_ * factor];
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
    virtual void push_back(T o) {
        // if our array is full
        if (size_ == capacity_) {
            expand_(size_ + 1);
        }
        items_[size_] = o;
        size_ += 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual T get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Removes all elements from the array.
     */
    virtual void clear() {
        size_ = 0;
        capacity_ = 10;
        delete[] items_;
        items_ = new T[capacity_];
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
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Array<T>* o = dynamic_cast<Array<T>*>(other);
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
     * Gets a copy of the array.
     * @return the copy of the array
     */
    virtual Array<T>* clone() {
        Array<T>* items_copy = new Array<T>();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }
};

template <typename T>
class Array<T*> : public Object {
   public:
    size_t size_;
    size_t capacity_;
    T** items_;

    /**
     * Creates an empty array. Inherits from Object
     * @return the array
     */
    Array() : Object() {
        size_ = 0;
        capacity_ = 10;
        items_ = new T*[capacity_];
    }

    /**
     * Deconstructs an instance of array.
     */
    ~Array() {
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
                T** new_items = new T*[capacity_ * factor];
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
    virtual void push_back(T* o) {
        // if our array is full
        if (size_ == capacity_) {
            expand_(size_ + 1);
        }
        items_[size_] = o;
        size_ += 1;
    }

    /**
     * Gets the element at a given index.
     * @arg i  index of the element to get
     * @return element at the index
     */
    virtual T* get(size_t i) {
        assert(i < size_);
        return items_[i];
    }

    /**
     * Removes all elements from the array.
     */
    virtual void clear() {
        size_ = 0;
        capacity_ = 10;
        delete[] items_;
        items_ = new T*[capacity_];
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
     * Checks if this array is equal to object given.
     * @arg other  object to compare
     * @return whether the two are equal
     */
    virtual bool equals(Object* other) {
        if (other == this) {
            return true;
        }
        Array<T*>* o = dynamic_cast<Array<T*>*>(other);
        if (o == nullptr || size_ != o->size_) {
            return false;
        }
        for (size_t i = 0; i < size_; i++) {
            if (*items_[i] != *o->get(i)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Gets a copy of the array.
     * @return the copy of the array
     */
    virtual Array<T*>* clone() {
        Array<T*>* items_copy = new Array<T*>();
        for (size_t i = 0; i < size_; i++) {
            items_copy->push_back(items_[i]);
        }
        return items_copy;
    }
};
