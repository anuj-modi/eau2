#include "../src/array.h"
#include "../src/string.h"
#include "catch.hpp"

// TEST_CASE("TRY SOMETHING", "[newarray]") {
//     Array<int>* a = new Array<int>();
//     Array<int>* b = new Array<int>();

//     for (int i = 0; i < 100; i++) {
//         a->push_back(i);
//         b->push_back(i);
//     }
//     REQUIRE(a->equals(b));
//     // REQUIRE(a == b);
// }

// TEST_CASE("TRY SOMETHING ELSE", "[newarray]") {
//     Array<Object*>* a = new Array<Object*>();
//     Array<Object*>* b = new Array<Object*>();

//     Object* o = new Object();

//     a->push_back(o);
//     b->push_back(o);

//     REQUIRE(a->equals(b));
//     // REQUIRE(a == b);
// }

// TEST_CASE("TRY PAirs", "[newarray]") {
//     Array<Pair*>* a = new Array<Pair*>();
//     Array<Pair*>* b = new Array<Pair*>();

//     Pair* p1 = new Pair(1, 2);
//     Pair* p2 = new Pair(1, 2);

//     a->push_back(p1);
//     b->push_back(p2);

//     REQUIRE(p1->equals(p2));
//     REQUIRE(a->equals(b));
// }

// tests push_back and get methods
TEST_CASE("push_back and get elements", "[array]") {
    Array<int> *l1 = new Array<int>();
    l1->push_back(-9);
    l1->push_back(10);
    REQUIRE(l1->get(0) == -9);
    REQUIRE(l1->get(1) == 10);
    REQUIRE(l1->size() == 2);

    String *s = new String("Hello");
    String *t = new String("World");
    Array<String *> *l2 = new Array<String *>();
    l2->push_back(s);
    l2->push_back(t);
    REQUIRE(l2->get(0)->equals(s));
    REQUIRE(l2->get(1)->equals(t));
    REQUIRE(l2->size() == 2);

    delete l1;
    delete s;
    delete t;
    delete l2;
}

// tests is_empty (is empty)
TEST_CASE("is_empty with empty array", "[array]") {
    Array<int> *l1 = new Array<int>();
    REQUIRE(l1->is_empty());
    REQUIRE(l1->size() == 0);

    Array<String *> *l2 = new Array<String *>();
    REQUIRE(l2->is_empty());
    REQUIRE(l2->size() == 0);

    delete l1;
    delete l2;
}

// tests is_empty (not empty)
TEST_CASE("is_empty with non-empty array", "[array]") {
    Array<int> *l1 = new Array<int>();
    l1->push_back(-9);
    l1->push_back(10);
    REQUIRE_FALSE(l1->is_empty());
    REQUIRE(l1->size() == 2);

    String *s = new String("Hello");
    String *t = new String("World");
    Array<String *> *l2 = new Array<String *>();
    l2->push_back(s);
    l2->push_back(t);
    REQUIRE_FALSE(l2->is_empty());
    REQUIRE(l2->size() == 2);

    delete l1;
    delete s;
    delete t;
    delete l2;
}

// tests clear method
TEST_CASE("clear", "[array]") {
    Array<int> *l1 = new Array<int>();
    l1->clear();
    REQUIRE(l1->is_empty());

    l1->push_back(-9);
    l1->push_back(10);
    l1->clear();
    REQUIRE(l1->is_empty());

    String *s = new String("Hello");
    String *t = new String("World");
    Array<String *> *l2 = new Array<String *>();
    l2->clear();
    REQUIRE(l2->is_empty());

    l2->push_back(s);
    l2->push_back(t);
    l2->clear();
    REQUIRE(l2->is_empty());

    delete l1;
    delete s;
    delete t;
    delete l2;
}

// test equals method
TEST_CASE("equals", "[array]") {
    Array<int> *l1 = new Array<int>();
    l1->push_back(-9);
    l1->push_back(10);
    Array<int> *l2 = new Array<int>();
    l2->push_back(-9);
    l2->push_back(10);
    REQUIRE(l1->equals(l2));

    String *s = new String("Hello");
    String *t = new String("World");
    Array<String *> *l3 = new Array<String *>();
    l3->push_back(s);
    l3->push_back(t);
    Array<String *> *l4 = new Array<String *>();
    l4->push_back(s);
    l4->push_back(t);
    REQUIRE(l3->equals(l4));
}

// test clone method
TEST_CASE("clone", "[array]") {
    Array<int> *l1 = new Array<int>();
    l1->push_back(-9);
    l1->push_back(10);
    Array<int> *l2 = l1->clone();
    REQUIRE(l1->equals(l2));

    String *s = new String("Hello");
    String *t = new String("World");
    Array<String *> *l3 = new Array<String *>();
    l3->push_back(s);
    l3->push_back(t);
    Array<String *> *l4 = l3->clone();
    REQUIRE(l3->equals(l4));
}
