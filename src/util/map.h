#pragma once
#include "array.h"
#include "object.h"
#include "pair.h"
#include "string.h"

/**
 * An object that represents a map to store keys and values.
 * Map does not own any objects passed to it.
 *
 * Authors: gomes.chri and modi.an
 */
class Map : public Object {
   public:
    size_t size_;
    Array** key_values_;
    size_t num_buckets_;
    double LOAD_FACTOR_ = 0.75;

    /* The constructor*/
    Map() {
        num_buckets_ = 500;
        size_ = 0;
        key_values_ = new Array*[num_buckets_];
        for (size_t i = 0; i < num_buckets_; i++) {
            key_values_[i] = new Array();
        }
    }

    /* The destructor*/
    virtual ~Map() {
        for (size_t i = 0; i < num_buckets_; i++) {
            for (size_t j = 0; j < key_values_[i]->size(); j++) {
                delete key_values_[i]->get(j);
            }
            delete key_values_[i];
        }
        delete[] key_values_;
    }

    /**
     * Determines the number of items in the map
     * @return the size of the map
     */
    size_t size() {
        return size_;
    }

    /**
     * Adds the given key value pair to the Map
     * @param key is the object to map the value to
     * @param value the object to add to the Map
     */
    void add(Object* key, Object* value) {
        if (size_ / num_buckets_ > LOAD_FACTOR_) {
            rehash_();
        }
        size_t hash_key = key->hash() % num_buckets_;
        bool updated = false;
        for (size_t i = 0; i < key_values_[hash_key]->size(); i++) {
            Pair* p = static_cast<Pair*>(key_values_[hash_key]->get(i));
            if (p->key->equals(key)) {
                p->value = value;
                updated = true;
            }
        }
        if (!updated) {
            key_values_[hash_key]->push_back(new Pair(key, value));
            size_ += 1;
        }
    }

    /**
     * Removes all the elements from the Map
     */
    void clear() {
        for (size_t i = 0; i < num_buckets_; i++) {
            for (size_t j = 0; j < key_values_[i]->size(); j++) {
                delete key_values_[i]->get(j);
            }
            delete key_values_[i];
        }
        delete[] key_values_;
        key_values_ = new Array*[num_buckets_];
        for (size_t i = 0; i < num_buckets_; i++) {
            key_values_[i] = new Array();
        }
        size_ = 0;
    }

    /**
     * Returns a copy of the Map
     * @return the copy of this map
     */
    Map* copy() {
        Map* new_map = new Map();
        Object** old_keys = keys();
        Object** old_values = values();
        for (size_t i = 0; i < size_; i++) {
            new_map->add(old_keys[i], old_values[i]);
        }
        delete[] old_keys;
        delete[] old_values;
        return new_map;
    }

    /**
     * Returns the value of the specified key
     * @param key the key to get the value from
     * @return the value associated with the key
     */
    Object* get(Object* key) {
        size_t hash_key = key->hash() % num_buckets_;
        if (key_values_[hash_key]->size() == 1) {
            return static_cast<Pair*>(key_values_[hash_key]->get(0))->value;
        } else {
            for (size_t i = 0; i < key_values_[hash_key]->size(); i++) {
                Pair* p = static_cast<Pair*>(key_values_[hash_key]->get(i));
                if (p->key->equals(key)) {
                    return p->value;
                }
            }
        }
        assert(false);
    }

    /**
     * Returns the value of the specified key
     * @param key the key to get the value from
     * @return the value associated with the key
     */
    bool contains(Object* key) {
        size_t hash_key = key->hash() % num_buckets_;
        if (key_values_[hash_key]->size() == 1) {
            return true;
        } else {
            for (size_t i = 0; i < key_values_[hash_key]->size(); i++) {
                Pair* p = static_cast<Pair*>(key_values_[hash_key]->get(i));
                if (p->key->equals(key)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Returns the Map's keys.
     */
    Object** keys() {
        Object** keys = new Object*[size_];
        size_t keys_found = 0;
        size_t curr_bucket = 0;
        while (keys_found < size_) {
            for (size_t i = 0; i < key_values_[curr_bucket]->size(); i++) {
                Pair* p = static_cast<Pair*>(key_values_[curr_bucket]->get(i));
                keys[keys_found] = p->key;
                keys_found += 1;
            }
            curr_bucket += 1;
        }
        return keys;
    }

    /**
     * Returns all the Map's values
     */
    Object** values() {
        Object** all_keys = keys();
        Object** all_values = new Object*[size_];
        for (size_t i = 0; i < size_; i++) {
            all_values[i] = get(all_keys[i]);
        }
        delete[] all_keys;
        return all_values;
    }

    /**
     * Removes the element with the specified key
     * @param key the key
     * @return the value of the element removed
     */
    Object* pop_item(Object* key) {
        size_t hash_key = key->hash();
        int val_index = -1;
        Object* val;
        if (key_values_[hash_key]->size() == 1) {
            Pair* p = static_cast<Pair*>(key_values_[hash_key]->get(0));
            val = p->value;
            delete p;
            key_values_[hash_key]->clear();
        } else {
            for (size_t i = 0; i < key_values_[hash_key]->size(); i++) {
                Pair* p = static_cast<Pair*>(key_values_[hash_key]->get(i));
                if (p->key->equals(key)) {
                    val = p->value;
                    val_index = i;
                }
            }
            assert(val_index != -1);
            delete key_values_[hash_key]->remove(val_index);
        }
        return val;
    }

    /**
     * Is this object equal to that object?
     * @param o is the object to compare equality to.
     * @return	whether this object is equal to that object.
     */
    virtual bool equals(Object* o) {
        assert(o != nullptr);
        Map* other = dynamic_cast<Map*>(o);
        if (size_ != other->size()) {
            return false;
        }
        Object** this_vals = values();
        Object** other_vals = other->values();
        for (size_t i = 0; i < size_; i++) {
            if (!this_vals[i]->equals(other_vals[i])) {
                delete[] this_vals;
                delete[] other_vals;
                return false;
            }
        }

        delete[] this_vals;
        delete[] other_vals;
        return true;
    }

    /**
     * Calculate this object's hash.
     * @return a natural number of a hash for this object.
     */
    virtual size_t hash() {
        size_t hash_val = 0;
        Object** vals = values();
        for (size_t i = 0; i < size_; i++) {
            hash_val += vals[i]->hash();
        }
        delete[] vals;
        return hash_val;
    }

    /**
     * Increases the bucket size and rehashes the elements.
     */
    void rehash_() {
        Object** curr_keys = keys();
        Object** curr_vals = values();
        size_t old_size = size_;
        for (size_t i = 0; i < num_buckets_; i++) {
            delete key_values_[i];
        }
        size_ = 0;
        num_buckets_ = num_buckets_ * 2;
        delete[] key_values_;
        key_values_ = new Array*[num_buckets_];
        for (size_t i = 0; i < old_size; i++) {
            add(curr_keys[i], curr_vals[i]);
        }
        delete[] curr_keys;
        delete[] curr_vals;
    }
};
