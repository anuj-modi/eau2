#pragma once
#include <arpa/inet.h>
#include <assert.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util/object.h"
#include "util/string.h"

/**
 * A class which helps convert between address string and byte representations
 */
class Address : public Object {
   public:
    String *ip_str_;
    uint32_t bytes_;

    /**
     * Creates an instance of address.
     * @arg ip  string literal representation of an ip
     */
    Address(const char *ip) : Object() {
        ip_str_ = new String(ip);
        struct sockaddr_in addr;
        assert(inet_pton(AF_INET, ip, &addr.sin_addr) == 1);
        bytes_ = addr.sin_addr.s_addr;
    }

    /**
     * Creates an instance of address.
     * @arg ip  string class representation of an ip
     */
    Address(String *ip) : Address(ip->c_str()) {}

    /**
     * Creates an instance of address.
     * @arg addr  byte form of an ip address
     */
    Address(uint32_t addr) : Object() {
        bytes_ = addr;
        char buf[INET_ADDRSTRLEN];
        assert(inet_ntop(AF_INET, &addr, buf, sizeof(buf)) != nullptr);
        ip_str_ = new String(buf);
    }

    /**
     * Creates an instance of address.
     * @arg addr  byte representation of an ip address w/ POSIX
     */
    Address(struct sockaddr_in addr) : Address(addr.sin_addr.s_addr) {}

    /**
     * Deconstructs an address.
     */
    virtual ~Address() {
        delete ip_str_;
    }

    /**
     * Gets ip address in bytes.
     * @return byte version of ip address
     */
    uint32_t as_bytes() {
        return bytes_;
    }

    /**
     * Gets string class version of ip
     * @return string with ip
     */
    String *as_str() {
        return ip_str_;
    }

    /**
     * Checks if this is equal to object provided.
     */
    virtual bool equals(Object *other) {
        if (other == this) {
            return true;
        }
        Address *o = dynamic_cast<Address *>(other);
        if (o == nullptr) {
            return false;
        }
        if (ip_str_->equals(o->ip_str_) && bytes_ == o->bytes_) {
            return true;
        }
        return false;
    }
};

/**
 * Wraps the system networking API and provides a CwC interface to sockets.
 */
class Socket : public Object {
   public:
    int sock_fd_;
    bool is_closed_;

    Socket() : Object() {
        sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        assert(sock_fd_ != -1);
        is_closed_ = false;
    }

    Socket(int socket_fd) : Object() {
        sock_fd_ = socket_fd;
        is_closed_ = false;
    }

    virtual ~Socket() {
        close_socket();
    }

    /**
     * Binds this socket to an address and port and begins listening for new connections.
     *
     * @param ip A string IP address to listen on
     * @param port a port number to bind to
     */
    void bind_and_listen(const char *ip, uint16_t port) {
        assert(false);
    }

    /**
     * Connects this socket to a remote port.
     * @param ip the remote IP address to connect to
     * @param port the port of the remote server to connect to
     */
    void connect_to_other(const char *ip, uint16_t port) {
        assert(false);
    }

    /**
     * Accept a new connection for this socket. Blocks if there are no waiting connections.
     * @returns a new Socket object specific to the new connection
     */
    ConnectionSocket *accept_connection() {
        assert(false);
        return nullptr;
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

    /**
     * Checks if this listening socket has any new connections.
     * @returns true if there are new connections waiting, false otherwise.
     */
    bool has_new_connections() {
        assert(false);
        return false;
    }

    /**
     * Checks if this connection socket has any bytes to be read.
     * @returns true if there are new bytes on the socket, false otherwise.
     */
    bool has_new_bytes() {
        assert(false);
        return false;
    }

    /**
     * Read bytes from the socket into the provided buffer.
     * @param buffer the buffer to place the read bytes into
     * @param num_bytes the number of bytes to read from the socket
     * @returns the number of bytes actually read from the socket
     */
    size_t recv_bytes(char *buffer, size_t num_bytes) {
        assert(false);
        return 0;
    }

    /**
     * Write bytes from the provided buffer into the socket to send to the remote host.
     * @param buffer the buffer to containing bytes to be written.
     * @param num_bytes the number of bytes to write from the buffer to the socket
     * @returns the number of bytes actually written to the socket
     */
    size_t send_bytes(const char *buffer, size_t num_bytes) {
        assert(false);
        return 0;
    }

    void close_socket() {
        if (!is_closed_) {
            close(sock_fd_);
        }
    }
};

class ListenSocket : public Socket {
   public:
    /**
     * Binds this socket to an address and port and begins listening for new connections.
     *
     * @param ip A string IP address to listen on
     * @param port a port number to bind to
     */
    void bind_and_listen(const char *ip, uint16_t port) {
        int opt = 1;
        // Forcefully attaching socket to the port 8080
        assert(setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) ==
               0);
        struct sockaddr_in adr;
        adr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &adr.sin_addr);
        adr.sin_port = htons(port);
        // Attaching socket to the port
        assert(bind(sock_fd_, (struct sockaddr *)&adr, sizeof(adr)) == 0);
        assert(listen(sock_fd_, 3) >= 0);
    }

    /**
     * Accept a new connection for this socket. Blocks if there are no waiting connections.
     * @returns a new Socket object specific to the new connection
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

class ConnectionSocket : public Socket {
   public:
    Address *peer_addr_;

    ConnectionSocket(int socket_fd, Address *peer_address) : Socket(socket_fd) {
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
    void connect_to_other(const char *ip, uint16_t port) {
        peer_addr_ = new Address(ip);
        struct sockaddr_in adr;
        adr.sin_family = AF_INET;
        adr.sin_addr.s_addr = peer_addr_->as_bytes();
        adr.sin_port = htons(port);
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
