#include "../src/array.h"
#include "catch.hpp"

/**
 * Determine if these two floats are equal with respect to eps.
 * @param f1 the first float to compare.
 * @param f2 the second float to compare.
 */
bool double_equal(double d1, double d2) {
    double eps = 0.0000001;
    if (d1 > d2) {
        return d1 - d2 < eps;
    } else {
        return d2 - d1 < eps;
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
    FloatArray *l3 = new FloatArray();
    l3->push_back(u);
    l3->push_back(v);
    REQUIRE((double_equal(l3->get(0), -3.3) && double_equal(l3->get(1), 4.4) && l3->size() == 2));

    double w = -3.3;
    double x = 4.4;
    DoubleArray *l5 = new DoubleArray();
    l5->push_back(w);
    l5->push_back(x);
    REQUIRE((double_equal(l5->get(0), -3.3) && double_equal(l5->get(1), 4.4) && l5->size() == 2));

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
    delete l5;
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
    FloatArray *l5 = new FloatArray();
    l5->push_back(u);
    l5->push_back(v);
    FloatArray *l6 = new FloatArray();
    l6->push_back_all(l5);
    REQUIRE(l6->equals(l5));

    double d1 = -2.2;
    double d2 = 1.0;
    DoubleArray *l14 = new DoubleArray();
    l14->push_back(d1);
    l14->push_back(d2);
    DoubleArray *l15 = new DoubleArray();
    l15->push_back_all(l14);
    REQUIRE(l14->equals(l15));

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

    FloatArray *l12 = new FloatArray();
    float f1 = 3.7;
    float f2 = 10.5;
    l12->push_back(f1);
    l12->push_back(f2);
    l5->push_back_all(l12);
    REQUIRE((double_equal(l5->get(2), 3.7) && double_equal(l5->get(3), 10.5) && l5->size() == 4));

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
    delete l14;
    delete l15;
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

    FloatArray *l3 = new FloatArray();
    float f = 5.6;
    float g = 7.8;
    l3->push_back(f);
    l3->add(0, g);
    REQUIRE((double_equal(l3->get(0), g) && double_equal(l3->get(1), f) && l3->size() == 2));

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
    l2->push_back(t);
    l2->add_all(0, l1);
    REQUIRE(l2->equals(l1));

    IntArray *l3 = new IntArray();
    l3->push_back(1);
    l3->push_back(2);
    IntArray *l4 = new IntArray();
    l4->push_back_all(l3);
    REQUIRE(l4->equals(l3));

    float u = -2.2;
    float v = 1.0;
    FloatArray *l5 = new FloatArray();
    l5->push_back(u);
    l5->push_back(v);
    FloatArray *l6 = new FloatArray();
    l6->push_back_all(l5);
    REQUIRE(l6->equals(l5));

    double d1 = -2.2;
    double d2 = 1.0;
    DoubleArray *l14 = new DoubleArray();
    l14->push_back(d1);
    l14->push_back(d2);
    DoubleArray *l15 = new DoubleArray();
    l15->push_back_all(l14);
    REQUIRE(l14->equals(l15));

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

    FloatArray *l12 = new FloatArray();
    float f1 = 3.7;
    float f2 = 10.5;
    l12->push_back(f1);
    l12->push_back(f2);
    l5->push_back_all(l12);
    REQUIRE((double_equal(l5->get(2), 3.7) && double_equal(l5->get(3), 10.5) && l5->size() == 4));

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
    delete l14;
    delete l15;
    delete l16;
    delete l17;
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

    FloatArray *l3 = new FloatArray();
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

    FloatArray *l3 = new FloatArray();
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

// void test_index_of() {
//     String *s = new String("Hello");
//     String *t = new String("World");
//     Array *l1 = new Array();
//     l1->push_back(s);
//     l1->push_back(t);

//     IntArray *l2 = new IntArray();
//     l2->push_back(6);
//     l2->add(0, 13);

//     FloatArray *l3 = new FloatArray();
//     float f = 5.6;
//     float g = 7.8;
//     l3->push_back(f);
//     l3->add(0, g);

//     BoolArray *l4 = new BoolArray();
//     l4->push_back(true);

//     String *hello = new String("Hello");
//     String *notIn = new String("NotIn");
//     printf("Test index_of for string (in array): ");
//     REQUIRE(l1->index_of(hello) == 0);

//     printf("Test index_of for string (not in array): ");
//     REQUIRE(l1->index_of(notIn) == 3);

//     printf("Test index_of for int (in array): ");
//     REQUIRE(l2->index_of(13) == 0);

//     printf("Test index_of for int (not in array): ");
//     REQUIRE(l2->index_of(7) == 3);

//     printf("Test index_of for float (in array): ");
//     REQUIRE(l3->index_of(7.8) == 0);

//     printf("Test index_of for float (not in array): ");
//     REQUIRE(l3->index_of(7) == 3);

//     printf("Test index_of for bool (in array): ");
//     REQUIRE(l4->index_of(true) == 0);

//     printf("Test index_of for bool (not in array): ");
//     REQUIRE(l4->index_of(false) == 2);

//     delete s;
//     delete t;
//     delete l1;
//     delete l2;
//     delete l3;
//     delete l4;
//     delete hello;
//     delete notIn;
// }

// void test_remove() {
//     String *s = new String("Hello");
//     String *t = new String("World");
//     Array *l1 = new Array();
//     l1->push_back(s);
//     l1->push_back(t);

//     IntArray *l2 = new IntArray();
//     l2->push_back(6);
//     l2->add(0, 13);

//     FloatArray *l3 = new FloatArray();
//     float f = 5.6;
//     float g = 7.8;
//     l3->push_back(f);
//     l3->add(0, g);

//     BoolArray *l4 = new BoolArray();
//     l4->push_back(true);
//     l4->push_back(false);

//     printf("Test remove for string (valid index): ");
//     REQUIRE(l1->remove(0)->equals(s) && l1->size() == 1);

//     // printf("Test remove for string (too big index): ");
//     // l1->remove(7);
//     // REQUIRE(l1->size() == 1);

//     printf("Test remove for int (valid index): ");
//     REQUIRE(l2->remove(0) == 13 && l2->size() == 1);

//     // printf("Test remove for int (too big index): ");
//     // l2->remove(7);
//     // REQUIRE(l2->size() == 1);

//     printf("Test remove for float (valid index): ");
//     REQUIRE(float_equal(l3->remove(0), 7.8) && l3->size() == 1);

//     // printf("Test remove for float (too big index): ");
//     // l3->remove(7);
//     // REQUIRE(l3->size() == 1);

//     printf("Test remove for bool (valid index): ");
//     REQUIRE(l4->remove(0) && l4->size() == 1);

//     // printf("Test remove for bool (too big index): ");
//     // l4->remove(7);
//     // REQUIRE(l4->size() == 1);

//     delete s;
//     delete t;
//     delete l1;
//     delete l2;
//     delete l3;
//     delete l4;
// }

// void test_set() {
//     String *s = new String("Hello");
//     String *t = new String("World");
//     String *u = new String("Hi");
//     Array *l1 = new Array();
//     l1->push_back(s);
//     l1->push_back(t);

//     IntArray *l2 = new IntArray();
//     l2->push_back(6);
//     l2->add(0, 13);

//     FloatArray *l3 = new FloatArray();
//     float f = 5.6;
//     float g = 7.8;
//     float h = 3.4;
//     l3->push_back(f);
//     l3->add(0, g);

//     BoolArray *l4 = new BoolArray();
//     l4->push_back(true);
//     l4->push_back(false);

//     // printf("Test set for string (too big index): ");
//     // REQUIRE(l1->set(5, u)->equals(u));

//     printf("Test set for string (valid index): ");
//     REQUIRE(l1->set(1, u)->equals(t) && l1->get(1)->equals(u));

//     // printf("Test set for int (too big index): ");
//     // REQUIRE(l2->set(5, 9) == 9);

//     printf("Test set for int (valid index): ");
//     REQUIRE(l2->set(1, 9) == 6 && l2->get(1) == 9);

//     // printf("Test set for float (too big index): ");
//     // REQUIRE(l3->set(5, h) == h);

//     printf("Test set for float (valid index): ");
//     REQUIRE(float_equal(l3->set(1, h), 5.6) && l3->get(1) == h);

//     // printf("Test set for bool (too big index): ");
//     // REQUIRE(l4->set(5, true));

//     printf("Test set for bool (valid index): ");
//     REQUIRE(!l4->set(1, true) && l4->get(1));

//     delete s;
//     delete t;
//     delete u;
//     delete l1;
//     delete l2;
//     delete l3;
//     delete l4;
// }

// void test_size() {
//     String *s = new String("Hello");
//     float f = 5.6;
//     Array *l1 = new Array();
//     IntArray *l2 = new IntArray();
//     FloatArray *l3 = new FloatArray();
//     BoolArray *l4 = new BoolArray();

//     printf("Test size for string (empty): ");
//     REQUIRE(l1->size() == 0);

//     printf("Test size for string (not empty): ");
//     l1->push_back(s);
//     REQUIRE(l1->size() == 1);

//     printf("Test size for int (empty): ");
//     REQUIRE(l2->size() == 0);

//     printf("Test size for int (not empty): ");
//     l2->push_back(5);
//     REQUIRE(l2->size() == 1);

//     printf("Test size for float (empty): ");
//     REQUIRE(l3->size() == 0);

//     printf("Test size for float (not empty): ");
//     l3->push_back(f);
//     REQUIRE(l3->size() == 1);

//     printf("Test size for bool (empty): ");
//     REQUIRE(l4->size() == 0);

//     printf("Test size for bool (not empty): ");
//     l4->push_back(true);
//     REQUIRE(l4->size() == 1);

//     delete s;
//     delete l1;
//     delete l2;
//     delete l3;
//     delete l4;
// }

// void test_hash() {
//     printf("Test hash for string: ");
//     String *s = new String("Hello");
//     String *t = new String("World");
//     Array *l1 = new Array();
//     l1->push_back(s);
//     l1->push_back(t);
//     Array *l2 = new Array();
//     l2->push_back_all(l1);
//     REQUIRE(l1->hash() == l2->hash());

//     printf("Test hash for int: ");
//     IntArray *l3 = new IntArray();
//     l3->push_back(1);
//     l3->push_back(2);
//     IntArray *l4 = new IntArray();
//     l4->push_back_all(l3);
//     REQUIRE(l3->hash() == l4->hash());

//     printf("Test hash for float: ");
//     float u = -2.2;
//     float v = 1.0;
//     FloatArray *l5 = new FloatArray();
//     l5->push_back(u);
//     l5->push_back(v);
//     FloatArray *l6 = new FloatArray();
//     l6->push_back_all(l5);
//     REQUIRE(l5->hash() == l6->hash());

//     printf("Test hash for bool: ");
//     BoolArray *l7 = new BoolArray();
//     l7->push_back(true);
//     l7->push_back(false);
//     BoolArray *l8 = new BoolArray();
//     l8->push_back_all(l7);
//     REQUIRE(l7->hash() == l8->hash());

//     delete s;
//     delete t;
//     delete l1;
//     delete l2;
//     delete l3;
//     delete l4;
//     delete l5;
//     delete l6;
//     delete l7;
//     delete l8;
// }
