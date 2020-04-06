#include "store/value.h"

#include "catch.hpp"
#include "util/string.h"

// test equal method
TEST_CASE("equal for value", "[value]") {
    String s("serialized data");
    String s_2("different serialized data");
    Value v_1(s.c_str(), s.size());
    Value* same = new Value(s.c_str(), s.size());
    Value* different = new Value(s_2.c_str(), s_2.size());

    REQUIRE(v_1.equals(same));
    REQUIRE_FALSE(v_1.equals(different));

    delete same;
    delete different;
}

// test hash method
TEST_CASE("hash a value", "[value]") {
    String s("serialized data");
    String s_2("different serialized data");
    Value v_1(s.c_str(), s.size());
    Value* same = new Value(s.c_str(), s.size());
    Value* different = new Value(s_2.c_str(), s_2.size());

    REQUIRE(v_1.hash() == same->hash());
    REQUIRE_FALSE(v_1.hash() == different->hash());

    delete same;
    delete different;
}

// test clone method
TEST_CASE("clone a value", "[value]") {
    String s("serialized data");
    Value v(s.c_str(), s.size());
    Value* v_clone = v.clone();
    REQUIRE(v.equals(v_clone));

    delete v_clone;
}

TEST_CASE("serialize deserialize value", "[value][serialize][deserialize]") {
    String st("serialized data");
    Value v(st.c_str(), st.size());

    Serializer s;
    v.serialize(&s);

    Deserializer d(s.get_bytes(), s.size());

    Value v2(&d);

    REQUIRE(v.equals(&v2));
}
