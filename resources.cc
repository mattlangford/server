#include "resources.hh"

#include <fstream>
#include <iterator>

//
// ############################################################################
//

std::string resource_type_to_string(const resource_type& type)
{
    switch (type)
    {
    case resource_type::TEXT:
        return "text/html";
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
    //
    std::ifstream file(base + path);
    std::string file_contents {std::istream_iterator<char>(file),
                               std::istream_iterator<char>()};
    resource->data = std::move(file_contents);

    return resource;
}
