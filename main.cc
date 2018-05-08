#include "http_server/http_server.hh"
#include "resources/file_resource.hh"

#include <iostream>
#include <vector>
#include <string>

constexpr auto SERVER_PATH = "/home/matt/Documents/server/test_server";

//
// ############################################################################
//

std::shared_ptr<resources::file_resource> load_file_resource(const std::string& path)
{
    return std::make_shared<resources::file_resource>(SERVER_PATH, path);
}

//
// ############################################################################
//

int main()
{
    http_server server(8080);

    server.add_resource(load_file_resource("/index.html"));

    server.start_server();

    while (true)
    {
        constexpr auto ONE_SECOND = std::chrono::duration<double>(1.0);
        std::this_thread::sleep_for(ONE_SECOND);
    }
}
