#pragma once
#include <cassert>
#include <unordered_map>

#include "connection.h"
#include "message.h"
#include "network.h"
#include "util/thread.h"

/**
 * Forward declaration of KVStore due to dependecy issues.
 */
class KVStore;

/**
 * Defines an API/abstraction for the network layer of eau2.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
class NetworkIfc : public Thread {
   public:
    bool keep_processing_;
    bool registration_done_;
    size_t node_num_;
    size_t total_nodes_;
    std::unordered_map<size_t, Connection*> connections_;
    std::unordered_map<size_t, Address*> peer_addresses_;
    ListenSocket* listen_sock_;
    Address my_addr_;
    KVStore* local_kv_;

    /**
     * NetworkIfc constructor to be called by all "clients" (node_num != 0)
     */
    NetworkIfc(Address* address, Address* controller, size_t node_num, size_t total_nodes)
        : Thread(),
          node_num_(node_num),
          total_nodes_(total_nodes),
          connections_(),
          my_addr_(address) {
        peer_addresses_[0] = new Address(controller);
        local_kv_ = nullptr;
        keep_processing_ = false;
        registration_done_ = false;
        listen_sock_ = new ListenSocket();
    }

    /**
     * Constructor meant to be called by "controller" (node_num == 0).
     */
    NetworkIfc(Address* address, size_t total_nodes)
        : NetworkIfc(address, address, 0, total_nodes) {}

    /**
     * Destructor for a NetworkIfc object.
     */
    virtual ~NetworkIfc() {
        for (std::pair<size_t, Connection*> p : connections_) {
            delete p.second;
        }
        for (std::pair<size_t, Address*> p : peer_addresses_) {
            delete p.second;
        }
        delete listen_sock_;
    }

    /**
     * Starts the processing loop for the network layer.
     */
    void start() override {
        assert(local_kv_ != nullptr);
        Thread::start();
        while (!keep_processing_) {
            // Wait until socket ready
        }
    }

    /**
     * The main logic for the network layer. It starts with the registration phase and then begins
     * the processing loop for new connections from other nodes in the network.
     */
    void run() override {
        assert(local_kv_ != nullptr);
        listen_sock_->bind_and_listen(&my_addr_);
        keep_processing_ = true;

        // registration phase
        if (node_num_ == 0) {
            process_client_registrations_();
        } else {
            register_with_controller_();
        }

        // ensure registration was successful
        assert(peer_addresses_.size() == total_nodes_);
        registration_done_ = true;

        printf("Finished registration\n");

        // process new connections from other clients
        while (keep_processing_) {
            if (listen_sock_->has_new_connections()) {
                ConnectionSocket* cs = listen_sock_->accept_connection();
                Connection* c = new Connection(cs, local_kv_);
                size_t node = total_nodes_;
                assert(cs->recv_bytes((char*)&node, sizeof(size_t)) > 0);
                assert(node != total_nodes_);
                c->start();
                connections_[node] = c;
            }
        }

        // begin graceful teardown
        if (node_num_ == 0) {
            Message kill_msg(MsgType::KILL);
            broadcast_(&kill_msg);
        }

        // end all connections and their threads
        for (std::pair<size_t, Connection*> p : connections_) {
            p.second->stop();
            p.second->join();
        }
    }

    /**
     * The "server" handling registration messages from "clients". It also sends out the directory
     * once all clients are connected.
     */
    void process_client_registrations_() {
        Connection* c = nullptr;
        // wait for registrations from everyone
        while (peer_addresses_.size() != total_nodes_) {
            ConnectionSocket* cs = listen_sock_->accept_connection();
            c = new Connection(cs, local_kv_);

            // Recv and handle registration message
            Deserializer* d_ptr = c->recv_message();
            Deserializer& d = *d_ptr;
            handle_register_message_(c, d);
        }

        // Send directory message out to everyone
        Directory dir(peer_addresses_);
        broadcast_(&dir);
        c->start();
    }

    /**
     * Process a REGISTER message.
     */
    void handle_register_message_(Connection* c, Deserializer& d) {
        MsgType m = d.get_msg_type();
        assert(m == MsgType::REGISTER);
        Register reg(&d);

        // Store peer address in address map
        peer_addresses_[reg.node_num_] = new Address(reg.client_addr_);
        connections_[reg.node_num_] = c;
    }

    /**
     * Sends message to all nodes.
     */
    void broadcast_(Message* message) {
        for (std::pair<size_t, Connection*> p : connections_) {
            // send all client addresses over server
            p.second->send_message(message);
        }
    }

    /**
     * The "clients" sending register messages to the "server". This method also handles the
     * server's Directory message.
     */
    void register_with_controller_() {
        // Connect to server
        ConnectionSocket* server_connection = new ConnectionSocket();
        server_connection->connect_to_other(peer_addresses_[0]);
        Connection* c = new Connection(server_connection, local_kv_);
        connections_[0] = c;

        // Send Server Register
        Register reg(new Address(&my_addr_), node_num_);
        c->send_message(&reg);

        // receive directory from server
        size_t num_bytes = 0;
        assert(server_connection->recv_bytes((char*)&num_bytes, sizeof(size_t)) > 0);
        char* buf = new char[num_bytes];
        assert(server_connection->recv_bytes(buf, num_bytes) > 0);
        Deserializer d(true, buf, num_bytes);
        handle_directory_message_(d);

        c->start();
    }

    /**
     * Stores the addresses given by the server.
     */
    void handle_directory_message_(Deserializer& d) {
        MsgType m = d.get_msg_type();
        assert(m == MsgType::DIRECTORY);
        // create Address array
        Directory dir(&d);
        assert(dir.client_addrs_.size() == total_nodes_);
        delete dir.client_addrs_[0];
        for (size_t i = 1; i < dir.client_addrs_.size(); i++) {
            Address* a = dir.client_addrs_[i];
            peer_addresses_[i] = a;
        }
    }

    /**
     * Stops the network layer's processing loop to gracefully shutdown.
     */
    void stop() {
        keep_processing_ = false;
    }

    /**
     * Blocks until the registration phase is over.
     */
    void wait_for_registration_() {
        while (!registration_done_) {
            // wait for registration phase to finish
        }
    }

    /**
     * A helper method which takes a node number and opens a connection to it if one does not
     * already exist.
     */
    void connect_to_node_(size_t node) {
        if (connections_.find(node) == connections_.end()) {
            assert(peer_addresses_.size() == total_nodes_);
            ConnectionSocket* cs = new ConnectionSocket();
            cs->connect_to_other(peer_addresses_.at(node));
            cs->send_bytes((char*)&node_num_, sizeof(size_t));
            Connection* c = new Connection(cs, local_kv_);
            c->start();
            connections_[node] = c;
        }
    }

    /**
     * Public API method which tells the specified node to put the given value at the given key.
     */
    void put_at_node(size_t node, Key& k, Value* v) {
        wait_for_registration_();
        assert(node_num_ != node);
        Put p(k, v);
        connect_to_node_(node);
        connections_.at(node)->send_message(&p);
    }

    /**
     * A helper method which waits for and processes a reply to a get or waitAndGet call.
     */
    Value* process_reply_(Connection* c) {
        // Wait for a reply to come back
        while (c->replies.size() == 0) {
            continue;
        }

        assert(c->replies.size() == 1);

        Reply* r = c->replies.front();
        Value* result = r->v_->clone();

        c->replies.pop();
        delete r;

        return result;
    }

    /**
     * Public API method which gets the data at the given key from another node.
     */
    Value* get_from_node(size_t node, Key& k) {
        wait_for_registration_();
        assert(node_num_ != node);
        Get g(k);

        connect_to_node_(node);
        Connection* c = connections_.at(node);
        assert(c->replies.size() == 0);
        c->send_message(&g);

        return process_reply_(c);
    }

    /**
     * Public API method which performs a waitAndGet for the data at the given key on another node.
     */
    Value* wait_and_get_from_node(size_t node, Key& k) {
        wait_for_registration_();
        assert(node_num_ != node);
        WaitAndGet g(k);

        connect_to_node_(node);
        Connection* c = connections_.at(node);
        assert(c->replies.size() == 0);
        c->send_message(&g);

        return process_reply_(c);
    }

    /**
     * Public API method which returns the number of nodes in the network.
     */
    size_t num_nodes() {
        return total_nodes_;
    }

    /**
     * Public API method which returns the node number of this node.
     */
    size_t this_node() {
        return node_num_;
    }

    /**
     * Sets the local kv pointer to the given kv store.
     */
    void set_kv(KVStore* kv) {
        assert(kv != nullptr);
        local_kv_ = kv;
    }
};
