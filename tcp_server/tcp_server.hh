#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace server
{

typedef int socket_handle;

///
/// Network server that will listen and trigger callbacks asynchronously
///
class tcp_server
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Creates the server, doesn't do much except set up the server_fd
    ///
    tcp_server();

    ///
    /// Binds this server to a specific port, this should be called EXACTLY once
    ///
    void bind_to_port(const uint16_t port);

    ///
    /// Add a new callback to the interface, anytime we get a new message from the interface, it will
    /// be forwarded to all the registered callbacks. Callbacks shouldn't be blocking, since they are
    /// run in the same thread as the listener.
    ///
    using callback = std::function<void(const socket_handle& fd, const std::vector<uint8_t>&)>;
    inline void register_callback(callback&& cb)
    {
        callbacks.emplace_back(std::move(cb));
    }

    ///
    /// Listens forever, triggers any callbacks that are registered on new data
    ///
    void listen_forever() const;

    ///
    /// Writes a vector of bytes onto the given socket
    ///
    static void write(const socket_handle& fd, const std::vector<uint8_t>& data);

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Throw an error using the error number (converted to a string)
    ///
    static void throw_errno(const std::string& message);

private: //////////////////////////////////////////////////////////////////////
    ///
    /// File descriptor for the server
    ///
    const socket_handle server_fd;

    ///
    /// Have we bound to a port already?
    ///
    bool bound;

    ///
    /// Callbacks to call when a full new data packet is received
    ///
    std::vector<callback> callbacks;
};
}
