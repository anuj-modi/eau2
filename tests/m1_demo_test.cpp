#include <thread>
#include "application/application.h"
#include "catch.hpp"
#include "dataframe/dataframe.h"
#include "store/key.h"

class Demo : public Application {
   public:
    Key main = Key("main", 0);
    Key verify = Key("verif", 0);
    Key check = Key("ck", 0);

    Demo(size_t idx, KVStore* kv) : Application(idx, kv) {}

    void run() override {
        switch (this_node()) {
            case 0:
                producer();
                break;
            case 1:
                counter();
                break;
            case 2:
                summarizer();
        }
    }

    void producer() {
        size_t SZ = 100 * 1000;
        double* vals = new double[SZ];
        double sum = 0;
        for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
        delete DataFrame::fromArray(&main, &kd, SZ, vals);
        delete DataFrame::fromScalar(&check, &kd, sum);
        delete[] vals;
    }

    void counter() {
        DataFrame* v = kd.waitAndGet(main);
        double sum = 0;
        for (size_t i = 0; i < 100 * 1000; ++i) sum += v->get_double(0, i);
        delete DataFrame::fromScalar(&verify, &kd, sum);
        delete v;
    }

    void summarizer() {
        DataFrame* result = kd.waitAndGet(verify);
        DataFrame* expected = kd.waitAndGet(check);
        assert(expected->get_double(0, 0) == result->get_double(0, 0));
        delete result;
        delete expected;
    }
};

TEST_CASE("their m1", "[application]") {
    KVStore kv;
    Demo d0(0, &kv);
    Demo d1(1, &kv);
    Demo d2(2, &kv);

    d2.start();
    d0.start();
    d1.start();

    d2.join();
    d0.join();
    d1.join();
}

TEST_CASE("validate their m1", "[application]") {
    KVStore kv;
    KDStore kd(&kv);
    Demo d0(0, &kv);
    Demo d1(1, &kv);

    Key verify = Key("verif", 0);
    Key check = Key("ck", 0);

    d0.start();
    d1.start();

    DataFrame* result = kd.waitAndGet(verify);
    DataFrame* expected = kd.waitAndGet(check);

    REQUIRE(expected->get_double(0, 0) == result->get_double(0, 0));

    d0.join();
    d1.join();

    delete result;
    delete expected;
}
