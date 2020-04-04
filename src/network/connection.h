#pragma once
#include <queue>

#include "message.h"
#include "network.h"
#include "store/key.h"
#include "store/kvstore.h"
#include "store/value.h"
#include "util/serial.h"
#include "util/thread.h"

class Connection : public Thread {
   public:
    ConnectionSocket* s_;
    KVStore* local_store_;
    bool keep_processing_;
    std::queue<Reply*> replies;

    Connection(ConnectionSocket* s, KVStore* kv) : Thread(), replies() {
        s_ = s;
        local_store_ = kv;
        keep_processing_ = true;
    }

    virtual ~Connection() {
        delete s_;
    }

    void stop() {
        // pln("trying to stop CONN");
        keep_processing_ = false;
    }

    void send_message(Message* m) {
        Serializer s;
        m->serialize(&s);
        size_t num_bytes = s.size();
        assert(s_->send_bytes((char*)&num_bytes, sizeof(size_t)) > 0);
        assert(s_->send_bytes(s.get_bytes(), num_bytes) > 0);
    }

    Deserializer recv_message_() {
        size_t num_bytes = 0;
        assert(s_->recv_bytes((char*)&num_bytes, sizeof(size_t)) > 0 && num_bytes > 0);
        char* buf = new char[num_bytes];
        assert(s_->recv_bytes(buf, num_bytes) > 0);
        return Deserializer(true, buf, num_bytes);
    }

    /**
     * Processes the bytes received from the socket.
     */
    void handle_status_message_(Deserializer& d) {
        Status s(&d);
        printf("%s\n", s.message_->c_str());
    }

    void handle_get_message_(Deserializer& d) {
        Get g(&d);
        Reply r(local_store_->get(g.k_));
        send_message(&r);
    }

    void handle_put_message_(Deserializer& d) {
        Put p(&d);
        local_store_->put(p.k_, p.v_->clone());
    }

    void handle_wait_and_get_message_(Deserializer& d) {
        WaitAndGet g(&d);
        Reply r(local_store_->waitAndGet(g.k_));
        send_message(&r);
    }

    void handle_reply_message_(Deserializer& d) {
        Reply* r = new Reply(&d);
        replies.push(r);
    }

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
        } else {
            assert(false);
        }
    }

    // handles messages from other nodes coming in
    void run() override {
        // pln("started executing CONN");
        while (keep_processing_) {
            if (s_->has_new_bytes()) {
                Deserializer d = recv_message_();
                handle_message_(d);
            }
        }
        // pln("finished executing CONN");
    }
};
