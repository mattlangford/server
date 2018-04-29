#include "tcp_server/server.hh"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <sstream>


namespace server
{

//
// ############################################################################
//

server::server()
    : server_fd(socket(PF_INET, SOCK_STREAM, 0)),
      bound(false)
{
    if (server_fd < 0)
    {
        throw_errno("Unable to create socket!");
    }

    // this allows us to reconnect immediately if we close out (rather than wait for the socket to reset)
    constexpr int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval));
}

//
// ############################################################################
//

void server::bind_to_port(const uint16_t port)
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

void server::listen_forever() const
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
        const int recv_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);

        //
        // Create a buffer that we'll use to read that file descriptor with
        //
        constexpr size_t RECV_BUF_SIZE = 100;
        uint8_t recv_buff[RECV_BUF_SIZE];
        size_t bytes_read = RECV_BUF_SIZE;

        //
        // Now while we continue to fill the buffer, push bytes into the final data vector
        //
        std::vector<uint8_t> data;
        while (bytes_read == RECV_BUF_SIZE)
        {
            bytes_read = read(recv_fd, &recv_buff, RECV_BUF_SIZE);
            std::copy(&recv_buff[0], &recv_buff[bytes_read], std::back_inserter(data));
        }

        //
        // If there's no data, don't bother with the callbacks
        //
        if (data.empty())
        {
            continue;
        }

        //
        // data buffer contains all the data from this read, dispatch to callbacks now!
        // NOTE: I'm assuming that no one else is modifying the list of callbacks (they've all been set
        // and wont be modified while this is running)
        //
        for (const callback& cb : callbacks)
        {
            cb(data);
        }
    }
}


//
// ############################################################################
//

void server::throw_errno(const std::string& message) const
{
    std::stringstream ss;
    ss << message << " Error: (" << errno << ") " << strerror(errno);

    throw std::runtime_error(ss.str());
}
}
