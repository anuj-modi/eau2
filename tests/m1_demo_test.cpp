#include <thread>

#include "application/application.h"
#include "catch.hpp"
#include "dataframe/dataframe.h"
#include "network/network_ifc.h"
#include "store/key.h"

class Demo : public Application {
   public:
    Key main = Key("main", 0);
    Key verify = Key("verif", 0);
    Key check = Key("ck", 0);
    size_t SZ = 1000;

    Demo(NetworkIfc& net) : Application(net) {}

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
        double* vals = new double[SZ];
        double sum = 0;
        for (size_t i = 0; i < SZ; ++i) sum += vals[i] = i;
        delete DataFrame::fromArray(&main, &kd_, SZ, vals);
        delete DataFrame::fromScalar(&check, &kd_, sum);
        delete[] vals;
    }

    void counter() {
        DataFrame* v = kd_.waitAndGet(main);
        double sum = 0;
        for (size_t i = 0; i < SZ; ++i) sum += v->get_double(0, i);
        delete DataFrame::fromScalar(&verify, &kd_, sum);
        delete v;
    }

    void summarizer() {
        DataFrame* result = kd_.waitAndGet(verify);
        DataFrame* expected = kd_.waitAndGet(check);
        assert(expected->get_double(0, 0) == result->get_double(0, 0));
        delete result;
        delete expected;
    }
};

TEST_CASE("their m1", "[m1]") {
    Address a0("127.0.0.1", 10000);
    Address a1("127.0.0.1", 10001);
    Address a2("127.0.0.1", 10002);

    NetworkIfc net0(&a0, 3);
    NetworkIfc net1(&a1, &a0, 1, 3);
    NetworkIfc net2(&a2, &a0, 2, 3);

    Demo d0(net0);
    Demo d1(net1);
    Demo d2(net2);

    d0.start();
    d2.start();
    d1.start();

    d2.join();
    d1.join();
    d0.join();
}