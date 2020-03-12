#include "../src/array.h"
#include "catch.hpp"

/**
 * Determine if these two floats are equal with respect to eps.
 * @param f1 the first float to compare.
 * @param f2 the second float to compare.
 */
static bool float_equal(float f1, float f2) {
    float eps = 0.0000001;
    if (f1 > f2) {
        return f1 - f2 < eps;
    } else {
        return f2 - f1 < eps;
    }
}

// tests that all push_backs and gets work
TEST_CASE("push_back and get", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    REQUIRE((l1->get(0)->equals(s) && l1->get(1)->equals(t) && l1->size() == 2));

    IntArray *l2 = new IntArray();
    l2->push_back(3);
    l2->push_back(4);
    REQUIRE((l2->get(0) == 3 && l2->get(1) == 4 && l2->size() == 2));

    float u = -3.3;
    float v = 4.4;
    DoubleArray *l3 = new DoubleArray();
    l3->push_back(u);
    l3->push_back(v);
    REQUIRE((float_equal(l3->get(0), -3.3) && float_equal(l3->get(1), 4.4) && l3->size() == 2));

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    l4->push_back(false);
    REQUIRE((l4->get(0) && !l4->get(1) && l4->size() == 2));

    StringArray *l6 = new StringArray();
    l6->push_back(s);
    l6->push_back(t);
    REQUIRE((l1->get(0)->equals(s) && l1->get(1)->equals(t) && l1->size() == 2));

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l6;
}

// test push_back_all for all types
TEST_CASE("push_back_all", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    Array *l2 = new Array();
    l2->push_back_all(l1);
    REQUIRE(l2->equals(l1));

    IntArray *l3 = new IntArray();
    l3->push_back(1);
    l3->push_back(2);
    IntArray *l4 = new IntArray();
    l4->push_back_all(l3);
    REQUIRE(l4->equals(l3));

    float u = -2.2;
    float v = 1.0;
    DoubleArray *l5 = new DoubleArray();
    l5->push_back(u);
    l5->push_back(v);
    DoubleArray *l6 = new DoubleArray();
    l6->push_back_all(l5);
    REQUIRE(l6->equals(l5));

    BoolArray *l7 = new BoolArray();
    l7->push_back(true);
    l7->push_back(false);
    BoolArray *l8 = new BoolArray();
    l8->push_back_all(l7);
    REQUIRE(l8->equals(l7));

    Array *l10 = new Array();
    String *s2 = new String("Hi");
    String *t2 = new String("Mom");
    l10->push_back(s2);
    l10->push_back(t2);
    l1->push_back_all(l10);
    REQUIRE((l1->get(2)->equals(s2) && l1->get(3)->equals(t2) && l1->size() == 4));

    IntArray *l11 = new IntArray();
    l11->push_back(4);
    l11->push_back(5);
    l3->push_back_all(l11);
    REQUIRE((l3->get(2) == 4 && l3->get(3) == 5 && l3->size() == 4));

    DoubleArray *l12 = new DoubleArray();
    float f1 = 3.7;
    float f2 = 10.5;
    l12->push_back(f1);
    l12->push_back(f2);
    l5->push_back_all(l12);
    REQUIRE((float_equal(l5->get(2), 3.7) && float_equal(l5->get(3), 10.5) && l5->size() == 4));

    BoolArray *l13 = new BoolArray();
    l13->push_back(true);
    l13->push_back(true);
    l7->push_back_all(l13);
    REQUIRE((l7->get(2) && l7->get(3) && l7->size() == 4));

    StringArray *l16 = new StringArray();
    l16->push_back(s);
    l16->push_back(t);
    StringArray *l17 = new StringArray();
    l17->push_back_all(l16);
    REQUIRE(l16->equals(l17));

    delete s;
    delete t;
    delete s2;
    delete t2;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    delete l6;
    delete l7;
    delete l8;
    delete l10;
    delete l11;
    delete l12;
    delete l13;
    delete l16;
    delete l17;
}

// test add for all types
TEST_CASE("add", "[array]") {
    Array *l1 = new Array();
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("Hi");
    l1->push_back(s);
    l1->add(0, t);
    REQUIRE((l1->get(0)->equals(t) && l1->get(1)->equals(s) && l1->size() == 2));

    IntArray *l2 = new IntArray();
    l2->push_back(6);
    l2->add(0, 13);
    REQUIRE((l2->get(0) == 13 && l2->get(1) == 6 && l2->size() == 2));

    DoubleArray *l3 = new DoubleArray();
    float f = 5.6;
    float g = 7.8;
    l3->push_back(f);
    l3->add(0, g);
    REQUIRE((float_equal(l3->get(0), g) && float_equal(l3->get(1), f) && l3->size() == 2));

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    l4->add(0, false);
    REQUIRE((!l4->get(0) && l4->get(1) && l4->size() == 2));

    StringArray *l5 = new StringArray();
    l5->push_back(s);
    l5->add(0, t);
    REQUIRE((l5->get(0)->equals(t) && l5->get(1)->equals(s) && l5->size() == 2));

    delete s;
    delete t;
    delete u;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
}

// test add_all for all types
TEST_CASE("add_all", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    Array *l2 = new Array();
    l2->push_back(s);
    l2->push_back(t);
    l1->add_all(0, l2);
    l1->remove(2);
    REQUIRE(l1->equals(l2));

    IntArray *l3 = new IntArray();
    l3->push_back(1);
    IntArray *l4 = new IntArray();
    l4->push_back(2);
    l4->push_back(3);
    l3->add_all(0, l4);
    l3->remove(2);
    REQUIRE(l4->equals(l3));

    float u = -2.2;
    float v = 1.0;
    DoubleArray *l5 = new DoubleArray();
    l5->push_back(u);
    DoubleArray *l6 = new DoubleArray();
    l6->push_back(u);
    l6->push_back(v);
    l5->add_all(0, l6);
    l5->remove(2);
    REQUIRE(l6->equals(l5));

    BoolArray *l9 = new BoolArray();
    l9->push_back(true);
    BoolArray *l10 = new BoolArray();
    l10->push_back(true);
    l10->push_back(false);
    l9->add_all(0, l10);
    l9->remove(2);
    REQUIRE(l10->equals(l9));

    StringArray *l11 = new StringArray();
    l11->push_back(s);
    StringArray *l12 = new StringArray();
    l12->push_back(s);
    l12->push_back(t);
    l11->add_all(0, l12);
    l11->remove(2);
    REQUIRE(l11->equals(l12));

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    delete l6;
    delete l9;
    delete l10;
    delete l11;
    delete l12;
}

// test clear method for all types
TEST_CASE("clear", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    l1->clear();
    REQUIRE(l1->size() == 0);

    l1->clear();
    REQUIRE(l1->size() == 0);

    IntArray *l2 = new IntArray();
    l2->push_back(6);
    l2->add(0, 13);
    l2->clear();
    REQUIRE(l2->size() == 0);

    l2->clear();
    REQUIRE(l2->size() == 0);

    DoubleArray *l3 = new DoubleArray();
    float f = 5.6;
    float g = 7.8;
    l3->push_back(f);
    l3->add(0, g);
    l3->clear();
    REQUIRE(l3->size() == 0);

    l3->clear();
    REQUIRE(l3->size() == 0);

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    l4->add(0, false);
    l4->clear();
    REQUIRE(l4->size() == 0);

    l4->clear();
    REQUIRE(l4->size() == 0);

    StringArray *l5 = new StringArray();
    l5->push_back(s);
    l5->push_back(t);
    l5->clear();
    REQUIRE(l5->size() == 0);

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
}

// test is_empty method for all types
TEST_CASE("is_empty", "[array]") {
    Array *l1 = new Array();
    REQUIRE(l1->is_empty());

    IntArray *l2 = new IntArray();
    REQUIRE(l2->is_empty());

    DoubleArray *l3 = new DoubleArray();
    REQUIRE(l3->is_empty());

    BoolArray *l4 = new BoolArray();
    REQUIRE(l4->is_empty());

    StringArray *l5 = new StringArray();
    REQUIRE(l5->is_empty());

    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
}

// test index_of method for all array types
TEST_CASE("index_of", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    String *hello = new String("Hello");
    String *notIn = new String("NotIn");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    REQUIRE(l1->index_of(hello) == 0);
    REQUIRE(l1->index_of(notIn) == 3);

    IntArray *l2 = new IntArray();
    l2->push_back(6);
    l2->add(0, 13);
    REQUIRE(l2->index_of(13) == 0);
    REQUIRE(l2->index_of(7) == 3);

    DoubleArray *l3 = new DoubleArray();
    float f = 5.6;
    float g = 7.8;
    l3->push_back(f);
    l3->add(0, g);
    REQUIRE(l3->index_of(g) == 0);
    REQUIRE(l3->index_of(7) == 3);

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    REQUIRE(l4->index_of(true) == 0);
    REQUIRE(l4->index_of(false) == 2);

    StringArray *l5 = new StringArray();
    l5->push_back(s);
    l5->push_back(t);
    REQUIRE(l5->index_of(hello) == 0);
    REQUIRE(l5->index_of(notIn) == 3);

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    delete hello;
    delete notIn;
}

// test remove method for all array types
TEST_CASE("remove", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    REQUIRE((l1->remove(0)->equals(s) && l1->size() == 1));

    IntArray *l2 = new IntArray();
    l2->push_back(6);
    l2->add(0, 13);
    REQUIRE((l2->remove(0) == 13 && l2->size() == 1));

    DoubleArray *l3 = new DoubleArray();
    float f = 5.6;
    float g = 7.8;
    l3->push_back(f);
    l3->add(0, g);
    REQUIRE((float_equal(l3->remove(0), g) && l3->size() == 1));

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    l4->push_back(false);
    REQUIRE((l4->remove(0) && l4->size() == 1));

    StringArray *l5 = new StringArray();
    l5->push_back(s);
    l5->push_back(t);
    REQUIRE((l5->remove(0)->equals(s) && l5->size() == 1));

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
}

// tests set method for all array types
TEST_CASE("set", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("Hi");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    REQUIRE((l1->set(1, u)->equals(t) && l1->get(1)->equals(u)));

    IntArray *l2 = new IntArray();
    l2->push_back(6);
    l2->add(0, 13);
    REQUIRE((l2->set(1, 9) == 6 && l2->get(1) == 9));

    DoubleArray *l3 = new DoubleArray();
    float f = 5.6;
    float g = 7.8;
    float h = 3.4;
    l3->push_back(f);
    l3->add(0, g);
    REQUIRE((float_equal(l3->set(1, h), 5.6) && l3->get(1) == h));

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    l4->push_back(false);
    REQUIRE((!l4->set(1, true) && l4->get(1)));

    StringArray *l5 = new StringArray();
    l5->push_back(s);
    l5->push_back(t);
    REQUIRE((l5->set(1, u)->equals(t) && l5->get(1)->equals(u)));

    delete s;
    delete t;
    delete u;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
}

// test size method for all array types
TEST_CASE("size", "[array]") {
    String *s = new String("Hello");
    float f = 5.6;
    Array *l1 = new Array();
    IntArray *l2 = new IntArray();
    DoubleArray *l3 = new DoubleArray();
    BoolArray *l4 = new BoolArray();
    StringArray *l5 = new StringArray();

    REQUIRE(l1->size() == 0);

    l1->push_back(s);
    REQUIRE(l1->size() == 1);

    REQUIRE(l2->size() == 0);

    l2->push_back(5);
    REQUIRE(l2->size() == 1);

    REQUIRE(l3->size() == 0);

    l3->push_back(f);
    REQUIRE(l3->size() == 1);

    REQUIRE(l4->size() == 0);

    l4->push_back(true);
    REQUIRE(l4->size() == 1);

    REQUIRE(l5->size() == 0);

    l5->push_back(s);
    REQUIRE(l5->size() == 1);

    delete s;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    
}

// test hash for all array types
TEST_CASE("hash", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    Array *l2 = new Array();
    l2->push_back_all(l1);
    REQUIRE(l1->hash() == l2->hash());

    IntArray *l3 = new IntArray();
    l3->push_back(1);
    l3->push_back(2);
    IntArray *l4 = new IntArray();
    l4->push_back_all(l3);
    REQUIRE(l3->hash() == l4->hash());

    float u = -2.2;
    float v = 1.0;
    DoubleArray *l5 = new DoubleArray();
    l5->push_back(u);
    l5->push_back(v);
    DoubleArray *l6 = new DoubleArray();
    l6->push_back_all(l5);
    REQUIRE(l5->hash() == l6->hash());

    BoolArray *l7 = new BoolArray();
    l7->push_back(true);
    l7->push_back(false);
    BoolArray *l8 = new BoolArray();
    l8->push_back_all(l7);
    REQUIRE(l7->hash() == l8->hash());

    StringArray *l9 = new StringArray();
    l9->push_back(s);
    l9->push_back(t);
    Array *l10 = new Array();
    l10->push_back_all(l9);
    REQUIRE(l9->hash() == l10->hash());

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    delete l6;
    delete l7;
    delete l8;
    delete l9;
    delete l10;
}

// test contains for all array types
TEST_CASE("contains", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    String *notIn = new String("NotIn");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    REQUIRE(l1->contains(s));
    REQUIRE_FALSE(l1->contains(notIn));

    IntArray *l2 = new IntArray();
    l2->push_back(6);
    l2->add(0, 13);
    REQUIRE(l2->contains(6));
    REQUIRE_FALSE(l2->contains(-1));

    DoubleArray *l3 = new DoubleArray();
    float f = 5.6;
    float g = 7.8;
    l3->push_back(f);
    l3->add(0, g);
    REQUIRE(l3->contains(5.6));
    REQUIRE_FALSE(l3->contains(-1.0));

    BoolArray *l4 = new BoolArray();
    l4->push_back(true);
    REQUIRE(l4->contains(true));
    REQUIRE_FALSE(l4->contains(false));

    StringArray *l5 = new StringArray();
    l5->push_back(s);
    l5->push_back(t);
    REQUIRE(l5->contains(s));
    REQUIRE_FALSE(l5->contains(notIn));

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    delete notIn;
}

// test clone method for all array types
TEST_CASE("clone", "[array]") {
    String *s = new String("Hello");
    String *t = new String("World");
    Array *l1 = new Array();
    l1->push_back(s);
    l1->push_back(t);
    Array *l2 = l1->clone();
    REQUIRE(l2->equals(l1));
    l2->push_back(s);
    REQUIRE((l2->size() == 3 && l1->size() == 2));

    IntArray *l3 = new IntArray();
    l3->push_back(1);
    l3->push_back(2);
    IntArray *l4 = l3->clone();
    REQUIRE(l4->equals(l3));
    l4->push_back(3);
    REQUIRE((l4->size() == 3 && l3->size() == 2));

    float u = -2.2;
    float v = 1.0;
    DoubleArray *l5 = new DoubleArray();
    l5->push_back(u);
    l5->push_back(v);
    DoubleArray *l6 = l5->clone();
    REQUIRE(l6->equals(l5));
    l6->push_back(4.5);
    REQUIRE((l6->size() == 3 && l5->size() == 2));

    BoolArray *l7 = new BoolArray();
    l7->push_back(true);
    l7->push_back(false);
    BoolArray *l8 = l7->clone();
    REQUIRE(l8->equals(l7));
    l8->push_back(true);
    REQUIRE((l8->size() == 3 && l7->size() == 2));

    StringArray *l16 = new StringArray();
    l16->push_back(s);
    l16->push_back(t);
    StringArray *l17 = l16->clone();
    REQUIRE(l16->equals(l17));
    l16->push_back(s);
    REQUIRE((l16->size() == 3 && l17->size() == 2));

    delete s;
    delete t;
    delete l1;
    delete l2;
    delete l3;
    delete l4;
    delete l5;
    delete l6;
    delete l7;
    delete l8;
    delete l16;
    delete l17;
}
