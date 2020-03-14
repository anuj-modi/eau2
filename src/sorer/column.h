// Lang::CwC
#pragma once

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "../column.h"
#include "../object.h"

/**
 * Enum for the different types of SoR columns this code supports.
 */
enum class ColumnType { STRING, INTEGER, DOUBLE, BOOL, UNKNOWN };

/**
 * Converts the given ColumnType to a string.
 * @param type the column type
 * @return A string representing this column type. Do not free or modify this string.
 */
const char* columnTypeToString(ColumnType type) {
    switch (type) {
        case ColumnType::STRING:
            return "STRING";
        case ColumnType::INTEGER:
            return "INTEGER";
        case ColumnType::DOUBLE:
            return "DOUBLE";
        case ColumnType::BOOL:
            return "BOOL";
        default:
            return "UNKNOWN";
    }
}

/**
 * Creates the right subclass of BaseColumn based on the given type.
 * @param type The type of column to create
 * @return The newly created column. Caller must free.
 */
Column* makeColumnFromType(ColumnType type) {
    switch (type) {
        case ColumnType::STRING:
            return new StringColumn();
        case ColumnType::INTEGER:
            return new IntColumn();
        case ColumnType::DOUBLE:
            return new DoubleColumn();
        case ColumnType::BOOL:
            return new BoolColumn();
        default:
            assert(false);
    }
}

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
    virtual ~ColumnSet() {
        for (size_t i = 0; i < getLength(); i++) {
            if (_columns[i] != nullptr) {
                delete _columns[i];
            }
        }
    }

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
    virtual void initializeColumn(size_t which, ColumnType type) {
        assert(which < getLength());
        assert(_columns[which] == nullptr);
        Column* col = makeColumnFromType(type);
        _columns[which] = col;
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
