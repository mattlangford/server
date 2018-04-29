#include "server.hh"

#include <iostream>
#include <vector>

//
// ############################################################################
//

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
    server::server s;
    s.bind_to_port(7777);

    server::server::callback dispatch_cb = [](const std::vector<uint8_t>& data)
    {
        callback({data.begin(), data.end()});
    };
    s.register_callback(std::move(dispatch_cb));

    s.listen_forever();
}
