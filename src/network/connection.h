#pragma once
#include <queue>

#include "message.h"
#include "network.h"
#include "store/key.h"
#include "store/value.h"
#include "util/lock.h"
#include "util/serial.h"
#include "util/thread.h"

/**
 * Forward declaration for KVStore due to dependecy issues.
 */
class KVStore;

/**
 * Represents an active connection to another node in the network.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
class Connection : public Thread {
   public:
    Lock l_;
    ConnectionSocket* s_;
    KVStore* local_store_;
    bool keep_processing_;
    std::queue<Reply*> replies;

    /**
     * Constructs a connection from a socket and a kv store.
     * @arg s the Socket this connection should use
     * @arg kv a pointer to the kvstore this connection should use as a datastore
     */
    Connection(ConnectionSocket* s, KVStore* kv) : Thread(), l_(), replies() {
        s_ = s;
        local_store_ = kv;
        keep_processing_ = true;
    }

    /**
     * Destructor for a connection.
     */
    virtual ~Connection() {
        delete s_;
    }

    /**
     * Stops a connection by ending the processing loop.
     */
    void stop() {
        keep_processing_ = false;
    }

    /**
     * Checks if the underlying socket has new bytes.
     * Uses the locks to ensure thread safety.
     * @returns true if the socket has new bytes to be read
     */
    bool socket_ready_() {
        l_.lock();
        bool result = s_->has_new_bytes();
        l_.unlock();
        return result;
    }

    /**
     * Sends a given message through the connection to the other node.
     * Uses the lock for thread safety.
     * @arg m the message to be sent
     */
    void send_message(Message* m) {
        Serializer s;
        m->serialize(&s);
        size_t num_bytes = s.size();
        l_.lock();
        assert(s_->send_bytes((char*)&num_bytes, sizeof(size_t)) > 0);
        assert(s_->send_bytes(s.get_bytes(), num_bytes) > 0);
        l_.unlock();
    }

    /**
     * Receives a message from the remote host. Will block until some data is received.
     * Uses the lock for thread safety.
     * @returns nullptr if 0 bytes were received or a deserializer object containing the received
     * bytes
     */
    Deserializer* recv_message_() {
        size_t num_bytes = 0;
        l_.lock();
        size_t num_bytes_received = s_->recv_bytes((char*)&num_bytes, sizeof(size_t));
        if (num_bytes_received == 0) {
            l_.unlock();
            return nullptr;
        }
        assert(num_bytes_received > 0 && num_bytes > 0);
        char* buf = new char[num_bytes];
        assert(s_->recv_bytes(buf, num_bytes) > 0);
        l_.unlock();
        return new Deserializer(true, buf, num_bytes);
    }

    /**
     * Processes the bytes received from the socket.
     */
    void handle_status_message_(Deserializer& d) {
        Status s(&d);
        printf("%s\n", s.message_->c_str());
    }

    /**
     * Handles an incoming Get message by quering the KV for the data and sending it back out.
     * @arg the deserializer containing the get message.
     */
    void handle_get_message_(Deserializer& d);

    /**
     * Handles an incoming Put message by calling put on the local KV store object.
     * @arg d the deserializer object containing the Put message.
     */
    void handle_put_message_(Deserializer& d);

    /**
     * Handles an incoming Get message by calling waitAndGet on the local KV and sending the reply
     * back out.
     * @arg the deserializer containing the WaitAndGet message.
     */
    void handle_wait_and_get_message_(Deserializer& d);

    /**
     * Handles a Reply message by pushing the provided message to the reply queue.
     * @arg d the deserializer containing the Reply Message.
     */
    void handle_reply_message_(Deserializer& d) {
        Reply* r = new Reply(&d);
        replies.push(r);
    }

    /** Handles a message by checking the message type and calling the appropriate helper handler.
     * @arg d the deserializer containing the message bytes
     */
    void handle_message_(Deserializer& d) {
        MsgType m = d.get_msg_type();
        if (m == MsgType::STATUS) {
            handle_status_message_(d);
        } else if (m == MsgType::GET) {
            handle_get_message_(d);
        } else if (m == MsgType::PUT) {
            handle_put_message_(d);
        } else if (m == MsgType::WAITANDGET) {
            handle_wait_and_get_message_(d);
        } else if (m == MsgType::REPLY) {
            handle_reply_message_(d);
        } else if (m == MsgType::KILL) {
            keep_processing_ = false;
        } else {
            assert(false);
        }
    }

    /**
     * A processing loop for incoming messages from the peer in the connection.
     */
    void run() override {
        while (keep_processing_) {
            if (s_->has_new_bytes()) {
                Deserializer* d = recv_message_();
                if (d == nullptr) {
                    return;
                }
                handle_message_(*d);
                delete d;
            }
        }
    }
};
