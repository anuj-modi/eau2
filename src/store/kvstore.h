#pragma once
#include <map>
#include "key.h"
#include "value.h"

class KVStore : public Object {
   public:
    std::map<Key*, Value*> items_;

    KVStore() : Object() {
        items_ = std::map<Key*, Value*>();
    }

    ~KVStore() {
        for (size_t i = 0; i < items_.size(); i++) {

        }
    }
};