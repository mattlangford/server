#include "http_server.hh"

#include <iostream>
#include <vector>
#include <string>

//
// ############################################################################
//

int main()
{
    http_server server(8080);

    http_server::http_resource homepage;
    homepage.url = "/index.html";
    homepage.data = "<html><head></head><body>Hello World!</body></html>";
    server.add_resource(std::move(homepage));

    server.start_server();

    while (true)
    {
        constexpr auto ONE_SECOND = std::chrono::duration<double>(1.0);
        std::this_thread::sleep_for(ONE_SECOND);
    }
}
