#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "tcp_server/tcp_message.hh"

typedef std::unordered_map<std::string, std::string> metadata_t;

struct general_header
{
    std::string type;
    std::string url;
    std::string http_version;

    ///
    /// Get a header from a string
    ///
    static general_header from_string(std::string data);
};

///
/// The most general form of request, this can be parsed into any other requests or be used to send data
///
struct general_message
{
    general_header header;
    metadata_t metadata;
    std::string body;

    ///
    /// Form a general message from a tcp message
    ///
    static general_message from_message(server::tcp_message message);

    ///
    ///
    ///
    server::tcp_message tcp_connection;
};

namespace requests
{

///
/// Request by the client for a new resource
///
struct GET
{
    std::string url;

    metadata_t metadata;

    ///
    /// Parse a GET request from a general message
    ///
    static GET from_general_message(general_message message);

    ///
    /// Handle to the message in case we need to parse more, or for when we have to respond
    ///
    server::tcp_message tcp_connection;
};

///
/// Used to post data to the server, format TBD
///
struct POST
{
    std::string url;

    metadata_t metadata;

    ///
    /// Parse a PUT request from a general message and a tcp message
    ///
    static POST from_general_message(general_message message);

    ///
    /// Handle to the message in case we need to parse more, or for when we have to respond
    ///
    server::tcp_message tcp_connection;
};
}

namespace responses
{

struct abstract_response
{
    virtual std::string get_response_code() const = 0;

    virtual ~abstract_response() = default;

    metadata_t metadata;
    std::string data;
};

///
/// The last thing we wanted to do was well received by the server
///
struct OK final : public abstract_response
{
    constexpr static auto CODE = "200 OK";
    inline virtual std::string get_response_code() const { return CODE; }
};

///
/// If a resource was not found, this is returned to tell the user they're garbage
///
struct NOT_FOUND final : public abstract_response
{
    constexpr static auto CODE = "404 Not Found";
    inline virtual std::string get_response_code() const { return CODE; }
};

///
/// If a resource doesn't support POST requests, this may be returned
///
struct NOT_ALLOWED final : public abstract_response
{
    constexpr static auto CODE = "405 Method Not Allowed";
    inline virtual std::string get_response_code() const { return CODE; }
};

///
/// Using one of the above types, response to the given message
///
std::string generate_response(const abstract_response& response);

}
