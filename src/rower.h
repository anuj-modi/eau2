#pragma once
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

/*******************************************************************************
 *  Rower::
 *  An interface for iterating through each row of a data frame. The intent
 *  is that this class should subclassed and the accept() method be given
 *  a meaningful implementation. Rowers can be cloned for parallel execution.
 */
class Rower : public Object {
   public:
    Rower() : Object() {}
    virtual ~Rower() {}

    /** This method is called once per row. The row object is on loan and
        should not be retained as it is likely going to be reused in the next
        call. The return value is used in filters to indicate that a row
        should be kept. */
    virtual bool accept(Row& r) {
        assert(false);
        return false;
    }

    /** Once traversal of the data frame is complete the rowers that were
        split off will be joined.  There will be one join per split. The
        original object will be the last to be called join on. The join method
        is reponsible for cleaning up memory. */
    virtual void join_delete(Rower* other) {
        assert(false);
    }

    virtual Rower* clone() {
        return nullptr;
    }
};

class PrintRower : public Rower {
   public:
    PrintRower() : Rower() {}
    virtual ~PrintRower() {}

    bool accept(Row& r) {
        PrintFielder pf = PrintFielder();
        pf.start(r.get_idx());
        for (size_t i = 0; i < r.width(); i++) {
            r.visit(i, pf);
            if (i < r.width() - 1) {
                printf(" ");
            }
        }
        pf.done();
        return true;
    }
};

class MaxRower : public Rower {
   public:
    int maxInt;
    String* maxStr;
    double maxFloat;
    bool maxBool;

    MaxRower() : Rower() {
        maxInt = INT_MIN;
        maxStr = nullptr;
        maxFloat = -DBL_MAX;
        maxBool = false;
    }

    virtual ~MaxRower() {}

    bool accept(Row& r) {
        MaxFielder mf = MaxFielder();
        mf.start(r.get_idx());

        for (size_t i = 0; i < r.width(); i++) {
            r.visit(i, mf);
        }
        mf.done();

        if (maxInt < mf.maxInt) {
            maxInt = mf.maxInt;
        }

        if (maxStr == nullptr || strcmp(maxStr->c_str(), mf.maxStr->c_str()) < 0) {
            maxStr = mf.maxStr;
        }

        if (maxFloat < mf.maxFloat) {
            maxFloat = mf.maxFloat;
        }

        if (mf.maxBool) {
            maxBool = true;
        }

        return true;
    }

    void join_delete(Rower* r) {
        MaxRower* mr = dynamic_cast<MaxRower*>(r);
        if (mr == nullptr) {
            return;
        }

        if (maxInt < mr->maxInt) {
            maxInt = mr->maxInt;
        }

        if (maxStr == nullptr || strcmp(maxStr->c_str(), mr->maxStr->c_str()) < 0) {
            maxStr = mr->maxStr;
        }

        if (maxFloat < mr->maxFloat) {
            maxFloat = mr->maxFloat;
        }

        if (mr->maxBool) {
            maxBool = true;
        }
        delete mr;
    }

    Rower* clone() {
        return new MaxRower();
    }
};

class ConcatStrRower : public Rower {
   public:
    StringColumn* concat_strs;

    ConcatStrRower() : Rower() {
        concat_strs = new StringColumn();
    }

    virtual ~ConcatStrRower() {
        delete concat_strs;
    }

    bool accept(Row& r) {
        ConcatStrFielder csf = ConcatStrFielder();
        csf.start(r.get_idx());
        for (size_t i = 0; i < r.width(); i++) {
            r.visit(i, csf);
        }
        csf.done();
        concat_strs->push_back(csf.str_buff.get());
        return true;
    }

    void join_delete(Rower* r) {
        ConcatStrRower* csf = dynamic_cast<ConcatStrRower*>(r);
        if (csf == nullptr) {
            return;
        }
        for (size_t i = 0; i < csf->concat_strs->size(); i++) {
            concat_strs->push_back(csf->concat_strs->get(i));
        }
        delete csf;
    }

    Rower* clone() {
        return new ConcatStrRower();
    }
};

class FilterGreater10 : public Rower {
   public:
    FilterGreater10() : Rower() {}
    ~FilterGreater10() {}

    bool accept(Row& r) {
        FilterGreater10Fielder filter_greater = FilterGreater10Fielder();
        r.visit(0, filter_greater);
        return filter_greater.result_;
    }
};
