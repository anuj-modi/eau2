#pragma once
#include <assert.h>
/*****************************************************************************
 * Fielder::
 * A field vistor invoked by Row.
 */

class Fielder : public Object {
   public:
    Fielder() : Object() {}
    virtual ~Fielder(){};

    /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
    virtual void start(size_t r) {}

    /** Called for fields of the argument's type with the value of the field. */
    virtual void accept(bool b) {}
    virtual void accept(double f) {}
    virtual void accept(int i) {}
    virtual void accept(String* s) {}

    /** Called when all fields have been seen. */
    virtual void done() {}
};

class PrintFielder : public Fielder {
   public:
    PrintFielder() : Fielder() {}
    virtual ~PrintFielder() {}

    void start(size_t r) {}

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b) {
        if (b) {
            printf("True");
        } else {
            printf("False");
        }
    }
    void accept(double f) {
        printf("%f", f);
    }

    void accept(int i) {
        printf("%d", i);
    }

    void accept(String* s) {
        printf("%s", s->c_str());
    }

    /** Called when all fields have been seen. */
    virtual void done() {
        printf("\n");
    }
};

class MaxFielder : public Fielder {
   public:
    int maxInt;
    String* maxStr;
    double maxFloat;
    bool maxBool;

    MaxFielder() : Fielder() {
        maxInt = 0;
        maxStr = nullptr;
        maxFloat = 0.0;
        maxBool = false;
    }

    virtual ~MaxFielder() {}

    void start(size_t r) {
        maxInt = 0;
        maxStr = nullptr;
        maxFloat = 0.0;
        maxBool = false;
    }

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b) {
        if (b) {
            maxBool = true;
        }
    }
    void accept(double f) {
        if (maxFloat < f) {
            maxFloat = f;
        }
    }

    void accept(int i) {
        if (maxInt < i) {
            maxInt = i;
        }
    }

    void accept(String* s) {
        if (maxStr == nullptr || strcmp(maxStr->c_str(), s->c_str()) < 0) {
            maxStr = s;
        }
    }

    /** Called when all fields have been seen. */
    virtual void done() {}
};

class ConcatStrFielder : public Fielder {
   public:
    StrBuff str_buff;

    ConcatStrFielder() : Fielder() {
        str_buff = StrBuff();
    }

    virtual ~ConcatStrFielder() {}

    void start(size_t r) {
        str_buff = StrBuff();
    }

    /** Called for fields of the argument's type with the value of the field. */
    void accept(bool b) {}
    void accept(double f) {}

    void accept(int i) {}

    void accept(String* s) {
        str_buff = str_buff.c(*s);
    }

    /** Called when all fields have been seen. */
    virtual void done() {}
};

class FilterGreater10Fielder : public Fielder {
   public:
    bool result_;
    FilterGreater10Fielder() : Fielder() {
        result_ = false;
    }
    virtual ~FilterGreater10Fielder() {}

    /** Called for fields of the argument's type with the value of the field. */
    void accept(int i) {
        if (i > 10) {
            result_ = true;
        }
    }
};
