#include "json/json.hh"

#include <iostream>

namespace
{
struct string_extractor_visitor
{
    //
    // ############################################################################
    //

    void operator()(const std::string& value, std::string& building)
    {
        building += "\"" + value + "\"";
    }

    //
    // ############################################################################
    //

    void operator()(const double& value, std::string& building)
    {
        building += std::to_string(value);
    }

    //
    // ############################################################################
    //

    void operator()(const bool& value, std::string& building)
    {
        building += std::to_string(value);
    }
    //
    // ############################################################################
    //

    void operator()(const json::map_type& value, std::string& building)
    {
        size_t elements_left = value.size();

        building += "{";
        for (const auto& element : value)
        {
            building += "\"" + element.first + "\": ";
            building += element.second.get_value_as_string();

            if (elements_left-- > 1)
            {
                building += ", ";
            }
        }
        building += "}";
    }

    //
    // ############################################################################
    //

    void operator()(const json::vector_type& value, std::string& building)
    {
        size_t elements_left = value.size();

        building += "[";
        for (const auto& element : value)
        {
            building += element.get_value_as_string();

            if (elements_left-- > 1)
            {
                building += ", ";
            }
        }
        building += "]";
    }

};
}

namespace json
{
std::string json::get_value_as_string() const
{
    std::string res;
    holder.apply_visitor<string_extractor_visitor, std::string&>(res);
    return res;
}

}
