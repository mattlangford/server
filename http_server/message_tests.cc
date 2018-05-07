#include "http_server/messages.hh"

#include <iostream>
#include <sstream>
#include <assert.h>

void check_GET_message()
{
    std::stringstream ss;
    ss << "GET /index.html HTTP/1.1\r\n"
       << "Host: localhost\r\n"
       << "User-Agent: my browser details\r\n"
       << "Connection: keep-alive\r\n"
       << "\r\n"
       << "";

    general_message message = general_message::from_string(ss.str());

    //
    // Make sure the header is good
    //
    assert(message.header == "GET /index.html HTTP/1.1");

    //
    // Make sure the metadata has been parsed, check one for validity
    //
    assert(message.metadata.find("Host") != message.metadata.cend());
    assert(message.metadata.find("User-Agent") != message.metadata.cend());
    assert(message.metadata.find("Connection") != message.metadata.cend());

    assert(message.metadata["Host"] == "localhost");

    //
    // Body should be empty
    //
    assert(message.body.empty());

    //
    // Parse the message and check GET specific fields
    //
    requests::GET parsed_message = requests::GET::from_general_message(std::move(message));

    assert(parsed_message.url == "/index.html");
    assert(parsed_message.http_version == "HTTP/1.1");

    assert(parsed_message.metadata["Host"] == "localhost");
}


int main()
{
    std::cout << "Running!\n";
    check_GET_message();
    std::cout << "Passed!\n";
}
