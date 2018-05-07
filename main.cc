#include "http_server.hh"
#include "resources.hh"

#include <iostream>
#include <vector>
#include <string>

constexpr auto SERVER_PATH = "/home/matt/Documents/server/test_server";

//
// ############################################################################
//

int main()
{
    http_server server(8080);

    server.add_resource(load_html_file(SERVER_PATH, "/index.html"));

    server.start_server();

    while (true)
    {
        constexpr auto ONE_SECOND = std::chrono::duration<double>(1.0);
        std::this_thread::sleep_for(ONE_SECOND);
    }
}
