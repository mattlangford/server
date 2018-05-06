#include "http_server.hh"
#include <string>

#include <iostream>
#include <functional>

//
// ############################################################################
//

http_server::http_server(const uint16_t port)
{
    //
    // Forward any messages we get to the generic request handler
    //
    auto dispatch = [this](const server::socket_handle& response_fd, const std::vector<uint8_t>& data){
        std::string string_data(data.begin(), data.end());
        handle_generic_request(response_fd, std::move(string_data));
    };
    server.register_callback(dispatch);

    server.bind_to_port(port);
}

//
// ############################################################################
//

void http_server::start_server()
{
    server_thread = std::thread(std::bind(&server::tcp_server::listen_forever, server));
}

//
// ############################################################################
//

void http_server::add_resource(http_resource resource)
{
    std::string url = resource.url;

    std::lock_guard<std::mutex> lock(resource_lock);
    std::cout << "Adding resource with URL: " << url << " (" << url.size() << ")\n";
    resources.emplace(url, std::make_shared<http_resource>(std::move(resource)));
}

//
// ############################################################################
//

const http_server::http_resource::ptr http_server::fetch_resource(const std::string& url) const
{
    std::lock_guard<std::mutex> lock(resource_lock);
    std::cout << "Fetch resource with URL: " << url << " (" << url.size() << ")\n";
    const auto resource_it = resources.find(url);

    for (auto& r : resources)
    {
        std::cout << r.first << ": " << (url == r.first) << "\n";
    }

    if (resource_it == resources.cend())
    {
        std::cout << "No resource with URL: " << url << "\n";
        return nullptr;
    }

    return resource_it->second;
}

//
// ############################################################################
//

void http_server::install_PUT_handler(PUT_handler&& handler)
{
    std::lock_guard<std::mutex> lock(handler_lock);
    PUT_handlers.emplace_back(std::move(handler));
}

//
// ############################################################################
//

void http_server::handle_generic_request(const server::socket_handle& response_fd, std::string data)
{
    general_message parsed_message = general_message::from_string(data);

    std::cout << "Got message! ==========================\n";
    std::cout << "header: " << parsed_message.header << "\n";
    std::cout << "metadata size: " << parsed_message.metadata.size() << "\n";
    std::cout << "body: " << parsed_message.body << "\n";
    std::cout << "=======================================\n";

    //
    // From the first 3 characters, we can determine what kind of message we should try to parse
    //
    try
    {
        const std::string message_type = data.substr(0, 3);
        if (message_type == "GET")
        {
            handle_GET_request(response_fd, requests::GET::from_general_message(std::move(parsed_message)));
        }
        else if(message_type == "PUT")
        {
            //handle_PUT_request();
        }
        else
        {
            throw std::runtime_error("No message parse-able with that header");
        }
    }
    catch (const std::runtime_error& err)
    {
        std::cout << "Exception caught in request handler: " << err.what()
            << "\nHeader: " << parsed_message.header << "\n";
    }
}

//
// ############################################################################
//

void http_server::handle_GET_request(const server::socket_handle& response_fd, requests::GET request)
{
    const http_resource::ptr resource = fetch_resource(request.url);
    if (resource == nullptr)
    {
        return;
    }

    std::cout << "Resource found! Serving...\n";
}
