#pragma once
#include "util/object.h"

class Row;

// /**
//  * Visitor that visits rows in a data frame.
//  * Author: gomes.chri, modi.an
//  */
// class Visitor : public Object {
//    public:
//     Visitor() {}

//     virtual ~Visitor() {}

//     /**
//      * Visits the given row.
//      * @arg r  the row
//      */
//     virtual void visit(Row &r) {}
// };

/**
 * Visitor that writes to the rows of a data frame.
 * Author: gomes.chri, modi.an
 */
class Writer : public Object {
   public:
    Writer() : Object() {}

    virtual ~Writer() {}

    /**
     * Visits the given row.
     * @arg r  the row
     */
    virtual void visit(Row &r) {}

    /**
     * Marks when the writer is done visiting the data frame.
     * @return true if done
     */
    virtual bool done() {
        return true;
    }
};

/**
 * Visitor that reads the rows in a data frame.
 * Author: gomes.chri, modi.an
 */
class Reader : public Object {
   public:
    Reader() : Object() {}

    virtual ~Reader() {}

    /**
     * Visits the given row.
     * @arg r  the row
     */
    virtual void visit(Row &r) {}
};