#pragma once
#include "resources/abstract_resource.hh"
#include "json/json.hh"

namespace resources
{

class json_resource : public abstract_resource
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    virtual ~json_resource() = default;

public: ///////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    inline virtual const std::string& get_resource_identifier() const = 0;

    ///
    /// Get a json object from the resource, this will be converted to a string and passed along
    ///
    virtual json::json get_json_resource() = 0;

    ///
    ///
    ///
    inline const std::string& get_resource_type() const override
    {
        static std::string resource_type {"text/json"};
        return resource_type;
    }

    ///
    ///
    ///
    inline std::string get_resource() override { return get_json_resource().get_value_as_string(); }
};
}
