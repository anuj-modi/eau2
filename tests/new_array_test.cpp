#include "../src/newarray.h"
#include "catch.hpp"

TEST_CASE("TRY SOMETHING", "[newarray]") {
    Array<int>* a = new Array<int>();
    Array<int>* b = new Array<int>();

    for (int i = 0; i < 100; i++) {
        a->push_back(i);
        b->push_back(i);
    }
    REQUIRE(a->equals(b));
    // REQUIRE(a == b);
}

TEST_CASE("TRY SOMETHING ELSE", "[newarray]") {
    Array<Object*>* a = new Array<Object*>();
    Array<Object*>* b = new Array<Object*>();

    Object* o = new Object();

    a->push_back(o);
    b->push_back(o);

    REQUIRE(a->equals(b));
    // REQUIRE(a == b);
}

TEST_CASE("TRY PAirs", "[newarray]") {
    Array<Pair*>* a = new Array<Pair*>();
    Array<Pair*>* b = new Array<Pair*>();

    Pair* p1 = new Pair(1, 2);
    Pair* p2 = new Pair(1, 2);

    a->push_back(p1);
    b->push_back(p2);

    REQUIRE(p1->equals(p2));
    REQUIRE(a->equals(b));
}
