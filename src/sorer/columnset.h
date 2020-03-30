#pragma once
// Lang::CwC
#include <assert.h>
#include <vector>
#include "dataframe/column.h"
#include "util/object.h"

/**
 * Represents a fixed-size set of columns of potentially different types.
 */
class ColumnSet : public Object {
   public:
    /** The array of columns */
    std::vector<Column*> _columns;
    /** The number of columns we have */
    // size_t _length;
    /**
     * Creates a new ColumnSet that can hold the given number of columns.
     * Caller must also call initializeColumn for each column to fully initialize this class.
     * @param num_columns The max number of columns that can be held
     */
    ColumnSet(size_t num_columns) : Object() {
        _columns = std::vector<Column*>();
        // _length = num_columns;
        for (size_t i = 0; i < num_columns; i++) {
            _columns.push_back(nullptr);
        }
    }
    /**
     * Destructor for ColumnSet
     */
    virtual ~ColumnSet() {}

    /**
     * Gets the number of columns that can be held in this ColumnSet.
     * @return The number of columns
     */
    virtual size_t getLength() {
        return _columns.size();
    }

    /**
     * Initializes the given column to the given type. Can only be called exactly once per index.
     * @param which The index for the column to initialize
     * @param type The type of column to create
     */
    virtual void initializeColumn(size_t which, ColumnType type, KVStore* store) {
        assert(which < getLength());
        assert(_columns[which] == nullptr);
        switch (type) {
            case ColumnType::STRING:
                _columns[which] = new StringColumn(store);
                break;
            case ColumnType::INTEGER:
                _columns[which] = new IntColumn(store);
                break;
            case ColumnType::DOUBLE:
                _columns[which] = new DoubleColumn(store);
                break;
            case ColumnType::BOOL:
                _columns[which] = new BoolColumn(store);
                break;
            default:
                assert(false);
        }
    }

    /**
     * Gets the column with the given index. initializeColumn must have been called for this index
     * first.
     * @param which The column index to retrieve
     * @return The column with the given index
     */
    virtual Column* getColumn(size_t which) {
        assert(which < getLength());
        assert(_columns[which] != nullptr);
        return _columns[which];
    }

    /**
     * Gets all of the columns in the column set.
     * @return the columns
     */
    std::vector<Column*> getColumns() {
        return _columns;
    }
};
