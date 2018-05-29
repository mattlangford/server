#include "logging.hh"

#include "http_server/http_server.hh"
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
    auto dispatch = [this](const server::socket_handle& response_fd, const std::vector<uint8_t>& data) {
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

void http_server::add_resource(resources::abstract_resource::ptr resource)
{
    std::string url = resource->get_resource_identifier();
    LOG_DEBUG("Adding resource with URL: " << url);

    std::lock_guard<std::mutex> lock(resource_lock);
    resources.emplace(std::move(url), std::move(resource));
}

//
// ############################################################################
//

const resources::abstract_resource::ptr http_server::fetch_resource(const std::string& url) const
{
    LOG_DEBUG("Fetch resource with URL: " << url);

    std::lock_guard<std::mutex> lock(resource_lock);
    const auto resource_it = resources.find(url);

    if (resource_it == resources.cend())
    {
        LOG_WARN("No resource with URL: " << url);
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
        LOG_ERROR("Exception caught in request handler: " << err.what()
            << "\nHeader: " << parsed_message.header);
    }
}

//
// ############################################################################
//

void http_server::handle_GET_request(const server::socket_handle& response_fd, requests::GET request)
{
    resources::abstract_resource::ptr resource = fetch_resource(request.url);
    if (resource == nullptr)
    {
        responses::NOT_FOUND response;
        LOG_DEBUG(response.get_response_code());
        server.write(response_fd, responses::generate_response(response));
        return;
    }

    responses::OK response;

    response.data = resource->get_resource();

    response.metadata["Connection"] = "close";
    response.metadata["Content-Type"] = resource->get_resource_type();
    response.metadata["Content-Length"] = response.data.size();

    LOG_DEBUG(response.get_response_code());
    server.write(response_fd, responses::generate_response(response));
    server.close(response_fd);
}
