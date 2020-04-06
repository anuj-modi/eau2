#pragma once
#include <cassert>
#include <unordered_map>
// #include <vector>

#include "connection.h"
#include "message.h"
#include "network.h"
// #include "store/kvstore.h"
#include "util/thread.h"

class KVStore;

class NetworkIfc : public Thread {
   public:
    bool keep_processing_;
    size_t node_num_;
    size_t total_nodes_;
    std::unordered_map<size_t, Connection*> connections_;
    std::unordered_map<size_t, Address*> peer_addresses_;
    ListenSocket* listen_sock_;
    Address my_addr_;
    KVStore* local_kv_;

    NetworkIfc(Address* address, Address* controller, size_t node_num, size_t total_nodes)
        : Thread(),
          node_num_(node_num),
          total_nodes_(total_nodes),
          connections_(),
          my_addr_(address) {
        peer_addresses_[0] = new Address(controller);
        local_kv_ = nullptr;
        keep_processing_ = false;
        listen_sock_ = new ListenSocket();
    }

    NetworkIfc(Address* address, size_t total_nodes)
        : NetworkIfc(address, address, 0, total_nodes) {}

    virtual ~NetworkIfc() {
        for (std::pair<size_t, Connection*> p : connections_) {
            delete p.second;
        }
        for (std::pair<size_t, Address*> p : peer_addresses_) {
            delete p.second;
        }
        delete listen_sock_;
    }

    void start() override {
        assert(local_kv_ != nullptr);
        Thread::start();
        while (!keep_processing_) {
            // Wait until socket ready
        }
    }

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

        if (node_num_ == 0) {
            Message kill_msg(MsgType::KILL);
            broadcast_(&kill_msg);
        }

        for (std::pair<size_t, Connection*> p : connections_) {
            p.second->stop();
            p.second->join();
        }
    }

    // The "server" handing registration messages from "clients"
    void process_client_registrations_() {
        // wait for registrations from everyone
        while (peer_addresses_.size() != total_nodes_) {
            ConnectionSocket* cs = listen_sock_->accept_connection();
            // TODO: change to use connection object
            char buf[1024];
            size_t num_bytes = cs->recv_bytes(buf, sizeof(buf));
            assert(num_bytes > 0);
            Deserializer d(buf, num_bytes);

            handle_register_message_(cs, d);
        }

        // Send directory message out to everyone
        Directory dir(peer_addresses_);
        broadcast_(&dir);
    }

    /**
     * Process a REGISTER message.
     */
    void handle_register_message_(ConnectionSocket* client, Deserializer& d) {
        MsgType m = d.get_msg_type();
        assert(m == MsgType::REGISTER);
        Register reg(&d);

        // Store peer address in address map
        peer_addresses_[reg.node_num_] = new Address(reg.client_addr_);

        // store peer connection in connections map
        Connection* c = new Connection(client, local_kv_);
        connections_[reg.node_num_] = c;
        c->start();
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

    // The "clients" registering with the "server"
    void register_with_controller_() {
        // Connect to server
        ConnectionSocket* server_connection = new ConnectionSocket();
        server_connection->connect_to_other(peer_addresses_[0]);

        // Send Server Register
        Register reg(new Address(&my_addr_), node_num_);
        Serializer s;
        reg.serialize(&s);
        // TODO: change to use send_message in connection
        Connection* c = new Connection(server_connection, local_kv_);
        assert(server_connection->send_bytes(s.get_bytes(), s.size()) > 0);

        // receive directory from server
        size_t num_bytes = 0;
        assert(server_connection->recv_bytes((char*)&num_bytes, sizeof(size_t)) > 0);
        char* buf = new char[num_bytes];
        assert(server_connection->recv_bytes(buf, num_bytes) > 0);
        Deserializer d(true, buf, num_bytes);
        handle_directory_message_(d);

        c->start();
        connections_[0] = c;
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

    void stop() {
        keep_processing_ = false;
    }

    void wait_for_registration_() {
        while (peer_addresses_.size() < total_nodes_) {
            // wait for registration phase to finish
        }
    }

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

    void put_at_node(size_t node, Key& k, Value* v) {
        wait_for_registration_();
        assert(node_num_ != node);
        Put p(k, v);
        connect_to_node_(node);
        connections_.at(node)->send_message(&p);
    }

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

    size_t num_nodes() {
        return total_nodes_;
    }

    size_t this_node() {
        return node_num_;
    }

    void set_kv(KVStore* kv) {
        assert(kv != nullptr);
        local_kv_ = kv;
    }
};
