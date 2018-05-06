#pragma once
#include <string>
#include <unordered_map>

///
/// The most general form of request, this can be parsed into any other requests or be used to send data
///
struct general_message
{
    std::string header;
    std::unordered_map<std::string, std::string> metadata;
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

    std::unordered_map<std::string, std::string> metadata;

    ///
    /// Parse a GET request from a general message
    ///
    static GET from_general_message(const general_message& message);
};

}
