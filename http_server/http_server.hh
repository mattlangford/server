#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

#include "tcp_server/tcp_server.hh"
#include "http_server/messages.hh"
#include "resources/abstract_resource.hh"

class http_server
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Construct with a port, 80 is the default but may require sudo
    ///
    http_server(const uint16_t port = 80);

public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Start/Stop the main server thread
    /// TODO: Actually allow the server to be stopped without control C
    ///
    void start_server();
    //void stop_server();

    ///
    /// Add a new resource to the server, this can be fetched on request
    ///
    void add_resource(resources::abstract_resource::ptr resource);

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Get the resource at the url, this could return null
    ///
    const resources::abstract_resource::ptr fetch_resource(const std::string& url) const;

    ///
    /// Callback used when new data on the socket arrives
    ///
    void handle_generic_request(server::tcp_message message);

    ///
    /// Callback to handle GET requests specifically, this will respond to the request if it can
    ///
    void handle_GET_request(requests::GET request);

    ///
    /// Callback to handle POST requests specifically, this will trigger callbacks
    ///
    void handle_POST_request(requests::POST request);

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Main server connection
    ///
    server::tcp_server server;

    ///
    /// Maps between url and resources, the shared pointer can be passed around
    ///
    mutable std::mutex resource_lock;
    std::unordered_map<std::string, resources::abstract_resource::ptr> resources;

    ///
    /// Thread handle for the server thread
    ///
    std::thread server_thread;
};
