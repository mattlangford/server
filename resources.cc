#include "resources.hh"

#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>

//
// ############################################################################
//

std::string resource_type_to_string(const resource_type& type)
{
    switch (type)
    {
    case resource_type::TEXT:
        return "text/html";
    case resource_type::ICON:
        return "image/icon";
    default:
        return "unknown";
    }
}

//
// ############################################################################
//

http_resource::ptr load_html_file(const std::string& base, const std::string& path)
{
    http_resource::ptr resource = std::make_shared<http_resource>();

    //
    // Make sure the path is always relative to root
    //
    if (path.at(0) != '/')
        resource->url = "/";
    resource->url += path;

    //
    // Since we're loading the file we know what it is
    //
    resource->encoding = resource_type::TEXT;

    //
    // Now load the data from the provided filepath
    // TODO: Do this more efficiently, seems like we're doing a lot of copying
    //
    std::ifstream file(base + path);
    std::stringstream file_ss;
    file_ss << file.rdbuf();
    resource->data = file_ss.str();

    return resource;
}

//
// ############################################################################
//

http_resource::ptr load_icon_file(const std::string& base, const std::string& path)
{
    http_resource::ptr resource = std::make_shared<http_resource>();

    //
    // Make sure the path is always relative to root
    //
    if (path.at(0) != '/')
        resource->url = "/";
    resource->url += path;

    //
    // Since we're loading the file we know what it is
    //
    resource->encoding = resource_type::ICON;

    //
    // Now load the data from the provided filepath
    //
    std::ifstream file(base + path, std::ios::binary);
    std::string file_contents {std::istream_iterator<char>(file),
                               std::istream_iterator<char>()};
    resource->data = std::move(file_contents);

    return resource;
}
