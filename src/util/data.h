#pragma once

union Payload {
    int i;
    double d;
    bool b;
    String* s;
};

class Data {
   public:
    bool missing;
    Payload payload;
};