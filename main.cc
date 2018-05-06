#include "tcp_server/tcp_server.hh"

#include <iostream>
#include <vector>
#include <string>

void callback(const std::string& recv)
{
    std::cout << "New Message!! (" << recv.size() << " bytes)\n";
    std::cout << "=============================================================\n";
    std::cout << recv;
    std::cout << "=============================================================\n";
}

//
// ############################################################################
//

int main()
{
    server::tcp_server s;
    s.bind_to_port(80);

    server::tcp_server::callback dispatch_cb = [](const server::socket_handle& handle, const std::vector<uint8_t>& data)
    {
        callback({data.begin(), data.end()});
    };
    s.register_callback(std::move(dispatch_cb));

    s.listen_forever();
}
