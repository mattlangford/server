#pragma once

#include <memory>
#include <string>

///
/// Different types of resources, this is transmitted in the metadata for HTTP responses
///
enum class resource_type : uint8_t
{
    TEXT,
    ICON
};

std::string resource_type_to_string(const resource_type& type);

namespace detail
{
struct abstract_resource
{

};
}

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

///
/// Load an HTML file off the disk. This will use the path as a the URL (but that can be
/// changed later)
///
http_resource::ptr load_html_file(const std::string& base, const std::string& path);

///
/// Load an .ico file off the disk. This will use the path as a the URL (but that can be
/// changed later)
///
http_resource::ptr load_icon_file(const std::string& base, const std::string& path);
