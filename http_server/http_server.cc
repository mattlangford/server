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
    auto dispatch = [this](server::tcp_message message) {
        handle_generic_request(std::move(message));
    };
    server.register_callback(std::move(dispatch));

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

void http_server::handle_generic_request(server::tcp_message message)
{
    general_message parsed_message = general_message::from_message(std::move(message));

    try
    {
        if (parsed_message.header.type == "GET")
        {
            handle_GET_request(requests::GET::from_general_message(std::move(parsed_message)));
        }
        else if (parsed_message.header.type == "POST")
        {
            //handle_POST_request(message);
        }
        else
        {
            throw std::runtime_error("No message parse-able with that header");
        }
    }
    catch (const std::runtime_error& err)
    {
        LOG_ERROR("Exception caught in request handler: " << err.what());
    }
}

//
// ############################################################################
//

void http_server::handle_GET_request(requests::GET request)
{
    resources::abstract_resource::ptr resource = fetch_resource(request.url);
    if (resource == nullptr)
    {
        responses::NOT_FOUND response;
        LOG_DEBUG(response.get_response_code());
        request.tcp_connection.write_to_socket(responses::generate_response(response));
        request.tcp_connection.close_socket();
        return;
    }

    responses::OK response;

    response.data = resource->get_resource();

    response.metadata["Connection"] = "close";
    response.metadata["Content-Type"] = resource->get_resource_type();
    response.metadata["Content-Length"] = std::to_string(response.data.size());

    LOG_DEBUG(response.get_response_code());
    request.tcp_connection.write_to_socket(responses::generate_response(response));
    request.tcp_connection.close_socket();
}

//
// ############################################################################
//

void http_server::handle_POST_request(requests::POST request)
{
    resources::abstract_resource::ptr resource = fetch_resource(request.url);
    if (resource == nullptr)
    {
        responses::NOT_FOUND response;
        LOG_DEBUG(response.get_response_code());
        request.tcp_connection.write_to_socket(responses::generate_response(response));
        request.tcp_connection.close_socket();
        return;
    }

    const auto send_response = [&](responses::abstract_response&& response){
        response.metadata["Connection"] = "close";
        response.metadata["Content-Type"] = resource->get_resource_type();
        response.metadata["Content-Length"] = response.data.size();

        LOG_DEBUG(response.get_response_code());
        request.tcp_connection.write_to_socket(responses::generate_response(response));
        request.tcp_connection.close_socket();
    };

    try
    {
        //
        // This may throw if the POST handler fails to do something right
        //
        if (resource->handle_post_request(std::move(request)))
        {
            send_response(responses::OK{});
        }
        else
        {
            LOG_ERROR("Post request failed for an unspecified reason!");
            send_response(responses::NOT_ALLOWED{});
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Post request failed! Exception: " << e.what());
        send_response(responses::NOT_ALLOWED{});
    }
}
