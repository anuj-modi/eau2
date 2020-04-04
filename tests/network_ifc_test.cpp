#include "network/network_ifc.h"

#include "catch.hpp"
#include "network/network.h"
#include "store/kvstore.h"

TEST_CASE("test registration phase", "[network_ifc]") {
    KVStore kv;
    Address controller_addr("127.0.0.1", 5555);
    NetworkIfc controller(&controller_addr, 2, &kv);

    Address client_addr("127.0.0.1", 5556);
    NetworkIfc client(&client_addr, &controller_addr, 1, 2, &kv);

    controller.start();
    client.start();

    controller.stop();
    client.stop();

    controller.join();
    client.join();

    REQUIRE(controller.peer_addresses_.size() == 2);
    REQUIRE(client.peer_addresses_.size() == 2);

    REQUIRE(client.peer_addresses_[0]->equals(&controller_addr));
    REQUIRE(client.peer_addresses_[1]->equals(&client_addr));

    REQUIRE(controller.peer_addresses_[0]->equals(&controller_addr));
    REQUIRE(controller.peer_addresses_[1]->equals(&client_addr));

    REQUIRE(controller.peer_addresses_[0]->equals(client.peer_addresses_[0]));
    REQUIRE(controller.peer_addresses_[1]->equals(client.peer_addresses_[1]));
}
