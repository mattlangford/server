#include "resources.hh"

std::string resource_type_to_string(const resource_type& type)
{
    switch (type)
    {
    case resource_type::TEXT:
        return "html/text";
    default:
        return "unknown";
    }
}
