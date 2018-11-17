#include "http_server/messages.hh"
#include "logging.hh"

#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>


namespace
{
constexpr auto NEWLINE = "\r\n";
constexpr size_t NEWLINE_SIZE = 2;

std::pair<std::string, std::string> parse_metadata_line(const std::string& data)
{
    const size_t split_index = data.find(": ");

    //
    // Key is pretty easy - everything up to the split index
    //
    std::string key = data.substr(0, split_index);

    //
    // For the value we need to parse everything after (the newline has already been removed)
    //
    constexpr size_t SPLIT_CHAR_LEN = 2;
    const size_t start_of_value = split_index + SPLIT_CHAR_LEN;
    const size_t length_of_value = data.size() - key.size() - SPLIT_CHAR_LEN;
    std::string value = data.substr(start_of_value, length_of_value);

    return std::make_pair(std::move(key), std::move(value));
}

//
// ############################################################################
//

std::string find_in_message(std::string to_find, server::tcp_message& message)
{
    std::string read_data;
    if (to_find.size() == 0)
    {
        return read_data;
    }

    while (true) // TODO timeout
    {
        read_data.resize(read_data.size() + 1);
        uint8_t* message_dest = reinterpret_cast<uint8_t*>(&read_data.back());

        message.read_into(message_dest, 1);
        if (read_data.size() < to_find.size())
            continue;

        //
        // Did we find the substring we're looking for? If so, break out
        //
        if (std::memcmp(message_dest - (to_find.size() - 1), to_find.data(), to_find.size()) == 0)
        {
            break;
        }
    }

    return read_data;
}

}

//
// ############################################################################
//

general_header general_header::from_string(std::string data)
{
    general_header result;

    const size_t first_space = data.find(' ');
    const size_t last_space = data.rfind(' ');

    if (first_space == last_space || first_space == std::string::npos)
    {
        throw std::runtime_error("Unable to parse header from string!");
    }

    result.type = data.substr(0, first_space);
    const size_t url_length = last_space - first_space - 1;
    result.url = data.substr(first_space + 1, url_length);
    result.http_version = data.substr(last_space + 1);

    return result;
}

//
// ############################################################################
//

general_message general_message::from_message(server::tcp_message message)
{
    general_message result;

    //
    // Parse out the header, it will be whatever is on the first line
    //
    result.header = general_header::from_string(find_in_message(NEWLINE, message));

    //
    // Now parse the arbitrary number of metadata arguments
    //
    while (true)
    {
        const std::string this_line = find_in_message(NEWLINE, message);

        //
        // No data on this line, we can break out since this was the end of the metadata
        //
        if (this_line.size() == NEWLINE_SIZE)
        {
            break;
        }

        result.metadata.emplace(parse_metadata_line(this_line));
    }

    result.tcp_connection = std::move(message);

    return result;
}

//
// ############################################################################
//

namespace requests
{

GET GET::from_general_message(general_message message)
{
    //
    // Make sure this is a GET request
    //
    constexpr auto GET_id = "GET";
    if (message.header.type != GET_id)
    {
        throw std::runtime_error("Trying to parse a non GET request.");
    }

    GET result;
    result.url = std::move(message.header.url);
    result.metadata = std::move(message.metadata);
    result.tcp_connection = std::move(message.tcp_connection);

    //
    // A single slash like this means the client wants the homepage, so let's replace it here
    //
    if (result.url == "/")
    {
        constexpr auto HOMEPAGE_URL = "/index.html";
        result.url = HOMEPAGE_URL;
    }

    return result;
}

//
// ############################################################################
//

// TODO: This is like exactly the same as the GET parsing...
POST POST::from_general_message(general_message message)
{
    //
    // Make sure this is a POST request
    //
    constexpr auto POST_id = "POST";
    if (message.header.type != POST_id)
    {
        throw std::runtime_error("Trying to parse a non POST request.");
    }

    POST result;
    result.url = std::move(message.header.url);
    result.metadata = std::move(message.metadata);
    result.tcp_connection = std::move(message.tcp_connection);

    return result;
}
}

//
// ############################################################################
//

namespace responses
{

std::string generate_response(const abstract_response& response)
{
    constexpr auto NEWLINE = "\r\n";
    constexpr auto HTTP_VERSION = "HTTP/1.0";

    std::stringstream response_message;

    //
    // Put in the header (which depends on the response type). The HTTP version will never change here
    //
    response_message << HTTP_VERSION << " " << response.get_response_code() << NEWLINE;

    //
    // All the metadata goes in next
    //
    for (const auto& metadata_element : response.metadata)
    {
        response_message << metadata_element.first << ": " << metadata_element.second << NEWLINE;
    }

    //
    // Then a blank line followed by the rest of the body
    //
    response_message << NEWLINE;
    response_message << response.data << NEWLINE;

    //
    // TODO: I'm making copies of this more than once! How rude of me
    //
    return response_message.str();
}
}
