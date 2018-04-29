#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>

#include <iostream>
#include <cstring>
#include <stdexcept>
#include <vector>


//
// ############################################################################
//

std::vector<char> read_until_empty(const int fd)
{
    constexpr size_t RECV_BUF_SIZE = 100;
    char recv_buff[RECV_BUF_SIZE];
    std::memset(recv_buff, 0, RECV_BUF_SIZE);
    size_t bytes_read = RECV_BUF_SIZE;

    std::vector<char> data;
    while (bytes_read == RECV_BUF_SIZE)
    {
        bytes_read = read(fd, &recv_buff, RECV_BUF_SIZE);
        std::copy(&recv_buff[0], &recv_buff[bytes_read], std::back_inserter(data));
    }

    return data;
}

//
// ############################################################################
//

void listen_forever(const int socket_fd)
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    while (true)
    {
        const int child_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_len);

        const auto recv_message = read_until_empty(child_fd);
        std::cout << "Bytes Read: " << recv_message.size() << "\n";
        for (char c : recv_message)
        {
            std::cout << c;
        }
    }
}

//
// ############################################################################
//

int main()
{
    //
    // Open the socket
    //
    const int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        throw std::runtime_error(std::string("Unable to connect to socket! Error: ") + strerror(errno));

    // this allows us to reconnect immediately if we close out (rather than wait for the socket to reset)
    constexpr int optval = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval));


    std::cout << "Socket connected.\n";

    //
    // Bind to the localhost interface on port 7777
    //
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(7777);
    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) != 0)
        throw std::runtime_error(std::string("Unable to connect to bind! Error: ") + strerror(errno));
    std::cout << "Bound to interface!\n";

    constexpr size_t NUM_QUEUED_MESSAGES = 10;
    if (listen(socket_fd, NUM_QUEUED_MESSAGES) < 0)
        throw std::runtime_error(std::string("Unable to start listening! Error: ") + strerror(errno));

    listen_forever(socket_fd);
}
