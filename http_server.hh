#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>

#include "tcp_server/tcp_server.hh"
#include "messages.hh"
#include "resources.hh"


class http_server
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Function to handle PUT requests, may change in the future
    ///
    typedef std::function<void(const std::string&)> PUT_handler;

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
    void add_resource(http_resource::ptr resource);

    template <typename... Args>
    inline void add_resource(Args...args)
    {
        add_resource(std::make_shared<http_resource>(std::move(args...)));
    }

    ///
    /// Add a new handler for PUT requests, don't block in the handler!
    ///
    void install_PUT_handler(PUT_handler&& handler);

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Get the resource at the url, this could return null
    ///
    const http_resource::ptr fetch_resource(const std::string& url) const;

    ///
    /// Callback used when new data on the socket arrives
    ///
    void handle_generic_request(const server::socket_handle& response_fd, std::string data);

    ///
    /// Callback to handle GET requests specifically, this will respond to the request if it can
    ///
    void handle_GET_request(const server::socket_handle& response_fd, requests::GET request);

    ///
    /// Callback to handle PUT requests specifically, this will trigger callbacks
    ///
    void handle_PUT_request();

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Main server connection
    ///
    server::tcp_server server;

    ///
    /// Maps between url and resources, the shared pointer can be passed around
    ///
    mutable std::mutex resource_lock;
    std::unordered_map<std::string, http_resource::ptr> resources;

    ///
    /// Callbacks to trigger on a new PUT request
    ///
    mutable std::mutex handler_lock;
    std::vector<PUT_handler> PUT_handlers;

    ///
    /// Thread handle for the server thread, with a shutdown bool
    ///
    std::thread server_thread;
};
