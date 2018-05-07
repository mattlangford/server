#pragma once

#include <sys/socket.h>

#include <functional>
#include <mutex>
#include <sstream>
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
    /// Writes the provided type to the socket, the type should have data and size functions
    ///
    template <typename T>
    static void write(const socket_handle& fd, const T& data)
    {
        constexpr int FLAGS = 0;
        const size_t bytes_sent = send(fd, data.data(), data.size(), FLAGS);

        if (bytes_sent != data.size())
        {
            std::stringstream ss;
            ss << "Wrote " << bytes_sent << ", but wanted to write " << data.size();
            throw std::runtime_error(ss.str());
        }
    }

    ///
    /// Write a single message and close the connection to the client
    ///
    template <typename T>
    static void write_single_message(const socket_handle& fd, const T& data)
    {
        write(fd, data);
        close(fd);
    }

    ///
    /// Signal that communications are done
    ///
    inline static void close(const socket_handle& fd)
    {
        shutdown(fd, SHUT_RDWR);
    }


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
