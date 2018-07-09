#include "http_server/messages.hh"
#include "logging.hh"

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
}

//
// ############################################################################
//

general_header general_header::from_string(const std::string& data)
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

general_message general_message::from_string(const std::string& data)
{
    general_message result;

    //
    // Header is pretty easy, since it's in the same spot
    //
    constexpr size_t start_of_header = 0;
    const size_t end_of_header = data.find(NEWLINE);
    LOG_DEBUG(data.substr(start_of_header, end_of_header - start_of_header));
    result.header = general_header::from_string(data.substr(start_of_header, end_of_header - start_of_header));

    //
    // Now parse the arbitrary number of arguments
    //
    size_t start_of_metdata_line = end_of_header + NEWLINE_SIZE;
    while (true)
    {
        //
        // Find the next metadata line and try to part it. If we reach two newlines in a row, that means
        // we're done with metadata and should move onto the body
        //
        const size_t end_of_metadata_line = data.find(NEWLINE, start_of_metdata_line);
        if (end_of_metadata_line == start_of_metdata_line)
        {
            break;
        }

        const size_t metadata_line_length = end_of_metadata_line - start_of_metdata_line;
        const std::string metadata_line = data.substr(start_of_metdata_line, metadata_line_length);

        result.metadata.emplace(parse_metadata_line(metadata_line));

        start_of_metdata_line = end_of_metadata_line + NEWLINE_SIZE;
    }

    //
    // The rest is the body!
    //
    result.body = data.substr(start_of_metdata_line + NEWLINE_SIZE);

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
    result.post_data = std::move(message.body);

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
