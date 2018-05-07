#pragma once

#include <memory>
#include <string>

enum class resource_type : uint8_t
{
    TEXT
};

std::string resource_type_to_string(const resource_type& type);

///
/// Resource we can offer up
///
struct http_resource
{
    std::string url;
    std::string data;
    resource_type encoding;

    typedef std::shared_ptr<http_resource> ptr;
};

