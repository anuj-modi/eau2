#include "object.h"

/**
 * Represents a key value pair in a map.
 *
 * Authors: gomes.chri and modi.an
 */
class Pair : public Object {
   public:
    Object* key;
    Object* value;

    /**
     * Creates a pair of objects.
     * @arg k  key of the pair
     * @arg v  value of the pair
     */
    Pair(Object* k, Object* v) : Object() {
        key = k;
        value = v;
    }

    /**
     * Deconstructs a pair.
     */
    virtual ~Pair() {}
};
