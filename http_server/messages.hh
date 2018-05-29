#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "tcp_server/tcp_server.hh"

typedef std::unordered_map<std::string, std::string> metadata_t;

///
/// The most general form of request, this can be parsed into any other requests or be used to send data
///
struct general_message
{
    std::string header;
    metadata_t metadata;
    std::string body;

    ///
    /// Form a general message from a raw string
    ///
    static general_message from_string(const std::string& data);
};

namespace requests
{

///
/// Request by the client for a new resource
///
struct GET
{
    std::string url;
    std::string http_version;

    metadata_t metadata;

    ///
    /// Parse a GET request from a general message
    ///
    static GET from_general_message(general_message message);
};

///
/// Used to put data to the server, format TBD
///
struct PUT
{
    std::string http_version;

    metadata_t metadata;

    std::string put_data;

    ///
    /// Parse a PUT request from a general message
    ///
    static PUT from_general_message(general_message message);
};
}

namespace responses
{

namespace detail
{
struct abstract_response
{
    virtual std::string get_response_code() const = 0;

    metadata_t metadata;
    std::string data;
};
}

///
/// The last thing we wanted to do was well received by the server
///
struct OK final : public detail::abstract_response
{
    constexpr static auto CODE = "200 OK";
    inline virtual std::string get_response_code() const { return CODE; }
};

///
/// If a resource was not found, this is returned to tell the user they're garbage
///
struct NOT_FOUND final : public detail::abstract_response
{
    constexpr static auto CODE = "404 Not Found";
    inline virtual std::string get_response_code() const { return CODE; }
};

///
/// Using one of the above types, response to the given message
///
std::string generate_response(const detail::abstract_response& response);

}
