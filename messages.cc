#include "messages.hh"

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

general_message general_message::from_string(const std::string& data)
{
    general_message result;

    //
    // Header is pretty easy, since it's in the same spot
    //
    constexpr size_t start_of_header = 0;
    const size_t end_of_header = data.find(NEWLINE);
    result.header = data.substr(start_of_header, end_of_header - start_of_header);

    //
    // Now parse the arbitrary number of arguments
    //
    size_t start_of_metdata_line = end_of_header + NEWLINE_SIZE;
    while (true)
    {
        //
        // Find the next metdata line and try to part it. If we reach two newlines in a row, that means
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
    constexpr size_t GET_id_len = 3;
    if (message.header.substr(0, GET_id_len) != GET_id)
    {
        std::stringstream ss;
        throw std::runtime_error("Trying to parse a non GET request.");
    }

    GET result;

    //
    // The url should be right after the start, grab that and the HTTP version
    //
    const size_t front_space = message.header.find(' ');
    const size_t end_space = message.header.rfind(' ');
    if (front_space == end_space)
    {
        throw std::runtime_error("Unable to parse header! Needs atleast two spaces.");
    }

    constexpr size_t SPACE_WIDTH = 1;
    const size_t url_length = end_space - front_space - SPACE_WIDTH;
    result.url = message.header.substr(front_space + SPACE_WIDTH, url_length);
    result.http_version = message.header.substr(end_space + SPACE_WIDTH);

    result.metadata = std::move(message.metadata);

    //
    // A single slash like this means give me the homepage, so let's replace it here
    //
    if (result.url == "/")
    {
        constexpr auto HOMEPAGE_URL = "/index.html";
        result.url = HOMEPAGE_URL;
    }

    return result;
}

}
