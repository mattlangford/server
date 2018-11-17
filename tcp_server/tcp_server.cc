#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <vector>

#include "tcp_server/tcp_server.hh"
#include "logging.hh"

namespace server
{

//
// ############################################################################
//

tcp_server::tcp_server()
    : server_fd(socket(PF_INET, SOCK_STREAM, 0)),
      bound(false)
{
    if (server_fd < 0)
    {
        throw_errno("Unable to create socket!");
    }

    // this allows us to reconnect immediately if we close out (rather than wait for the socket to reset)
    constexpr int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval)) < 0)
    {
        throw_errno("Unable to set socket operations!");
    }
}

//
// ############################################################################
//

void tcp_server::bind_to_port(const uint16_t port)
{
    if (bound)
    {
        throw std::runtime_error("Trying to rebind server that's already bound to a port!");
    }

    //
    // Bind to the localhost interface on port 7777
    //
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);
    if (bind(server_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) != 0)
        throw_errno("Unable to bind to port!");

    bound = true;
}

//
// ############################################################################
//

void tcp_server::listen_forever() const
{
    if (bound == false)
    {
        throw std::runtime_error("Can't listen on an unbound port!");
    }

    //
    // Start listening on the bound port
    //
    constexpr size_t NUM_QUEUED_MESSAGES = 100;
    if (listen(server_fd, NUM_QUEUED_MESSAGES) < 0)
        throw_errno("Unable to start listening on port!");

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    while (true)
    {
        //
        // Accept a new message from the socket, keep track of who wrote to us
        //
        const socket_handle recv_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);

        auto mutex = std::make_shared<std::mutex>();
        auto data = std::make_shared<std::vector<uint8_t>>();

        //
        // data buffer contains all the data from this read, dispatch to callbacks now!
        // NOTE: I'm assuming that no one else is modifying the list of callbacks (they've all been set
        // and wont be modified while this is running)
        //
        for (const callback& cb : callbacks)
        {
            tcp_message message(recv_fd, mutex, data);
            cb(std::move(message));
        }
    }
}

//
// ############################################################################
//

void tcp_server::throw_errno(const std::string& message)
{
    std::stringstream ss;
    ss << message << " Error: (" << errno << ") " << strerror(errno);

    throw std::runtime_error(ss.str());
}
}
