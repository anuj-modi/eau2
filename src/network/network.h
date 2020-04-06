#pragma once
#include <assert.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "address.h"
#include "util/object.h"
#include "util/serial.h"
#include "util/string.h"

/**
 * Wraps the system networking API and provides a CwC interface to sockets.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
class BaseSocket : public Object {
   public:
    int sock_fd_;
    bool is_closed_;

    BaseSocket() : Object() {
        sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        assert(sock_fd_ != -1);
        is_closed_ = false;
    }

    BaseSocket(int socket_fd) : Object() {
        sock_fd_ = socket_fd;
        is_closed_ = false;
    }

    virtual ~BaseSocket() {
        close_socket();
    }

    /**
     * Polls the underlying socket object for any new activity
     */
    bool is_ready_() {
        struct pollfd pfd;
        pfd.fd = sock_fd_;
        pfd.events = POLLIN;
        int result = poll(&pfd, 1, 0);
        assert(result >= 0);
        return result != 0 && pfd.revents == POLLIN;
    }

    void close_socket() {
        if (!is_closed_) {
            close(sock_fd_);
        }
    }
};

/**
 * A Socket object specifically for communicating with a peer over the network. Can send and receive
 * data. Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
class ConnectionSocket : public BaseSocket {
   public:
    Address *peer_addr_;

    ConnectionSocket() : BaseSocket() {}

    ConnectionSocket(int socket_fd, Address *peer_address) : BaseSocket(socket_fd) {
        peer_addr_ = peer_address;
    }

    virtual ~ConnectionSocket() {
        delete peer_addr_;
    }

    /**
     * Connects this socket to a remote port.
     * @param ip the remote IP address to connect to
     * @param port the port of the remote server to connect to
     */
    void connect_to_other(Address *peer) {
        peer_addr_ = new Address(peer);
        struct sockaddr_in adr;
        adr.sin_family = AF_INET;
        adr.sin_addr.s_addr = peer_addr_->ip_bytes();
        adr.sin_port = htons(peer_addr_->port());
        assert(connect(sock_fd_, (struct sockaddr *)&adr, sizeof(adr)) == 0);
    }

    /**
     * Checks if this connection socket has any bytes to be read.
     * @returns true if there are new bytes on the socket, false otherwise.
     */
    bool has_new_bytes() {
        return is_ready_();
    }

    /**
     * Read bytes from the socket into the provided buffer.
     * @param buffer the buffer to place the read bytes into
     * @param num_bytes the number of bytes to read from the socket
     * @returns the number of bytes actually read from the socket
     */
    size_t recv_bytes(char *buffer, size_t num_bytes) {
        int bytes_received = recv(sock_fd_, buffer, num_bytes, 0);
        assert(bytes_received >= 0);
        return bytes_received;
    }

    /**
     * Write bytes from the provided buffer into the socket to send to the remote host.
     * @param buffer the buffer to containing bytes to be written.
     * @param num_bytes the number of bytes to write from the buffer to the socket
     * @returns the number of bytes actually written to the socket
     */
    size_t send_bytes(const char *buffer, size_t num_bytes) {
        int bytes_sent = send(sock_fd_, buffer, num_bytes, 0);
        assert(bytes_sent != -1);
        return bytes_sent;
    }
};

/**
 * A Socket object specifically for listening for new connections from other hosts. When a
 * connection is received, a new ConnectionSocket object is created for that connection.
 * Authors: gomes.chri@husky.neu.edu and modi.an@husky.neu.edu
 */
class ListenSocket : public BaseSocket {
   public:
    ListenSocket() : BaseSocket() {}

    /**
     * Binds this socket to an address and port and begins listening for new connections.
     *
     * @param ip A string IP address to listen on
     * @param port a port number to bind to
     */
    void bind_and_listen(Address *local) {
        int opt = 1;
        // Forcefully attaching socket to the port 8080
        assert(setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) ==
               0);
        struct sockaddr_in adr;
        adr.sin_family = AF_INET;
        adr.sin_addr.s_addr = local->ip_bytes();
        adr.sin_port = htons(local->port());
        // Attaching socket to the port
        assert(bind(sock_fd_, (struct sockaddr *)&adr, sizeof(adr)) == 0);
        assert(listen(sock_fd_, 3) >= 0);
    }

    /**
     * Accept a new connection for this socket. Blocks if there are no waiting connections.
     * @returns a new ConnectionSocket object specific to the new connection
     */
    ConnectionSocket *accept_connection() {
        struct sockaddr_in addr;
        socklen_t size = sizeof(addr);
        int new_connection_fd = accept(sock_fd_, (struct sockaddr *)&addr, &size);
        assert(new_connection_fd != -1);
        return new ConnectionSocket(new_connection_fd, new Address(addr));
    }

    /**
     * Checks if this listening socket has any new connections.
     * @returns true if there are new connections waiting, false otherwise.
     */
    bool has_new_connections() {
        return is_ready_();
    }
};
