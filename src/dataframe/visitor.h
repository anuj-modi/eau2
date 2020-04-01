#pragma once
#include "util/object.h"

class Row;

/**
 * Visitor that visits rows in a data frame.
 * Author: gomes.chri, modi.an
 */
class Visitor : public Object {
   public:
    Visitor() {}

    virtual ~Visitor() {}

    /**
     * Visits the given row.
     * @arg r  the row
     */
    virtual void visit(Row &r) {}
};

/**
 * Visitor that writes to the rows of a data frame.
 * Author: gomes.chri, modi.an
 */
class Writer : public Visitor {
   public:
    Writer() : Visitor() {}

    virtual ~Writer() {}

    /**
     * Marks when the writer is done visiting the data frame.
     * @return true if done
     */
    virtual bool done() {
        return true;
    }

    // TODO find out what this does
    void next() {}
};

/**
 * Visitor that reads the rows in a data frame.
 * Author: gomes.chri, modi.an
 */
class Reader : public Visitor {
   public:
    Reader() : Visitor() {}

    virtual ~Reader() {}
};