#pragma once
#include "util/object.h"

/**
 * Visitor that visits rows in a data frame.
 * Author: gomes.chri, modi.an
 */
class Visitor : public Object {
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

    /**
     * Marks when the writer is done visiting the data frame.
     * @return true if done
     */
    virtual bool done() {
        return true;
    }

    void next() {}
};

/**
 * Visitor that reads the rows in a data frame.
 * Author: gomes.chri, modi.an
 */
class Reader : public Visitor {};