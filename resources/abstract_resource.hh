#pragma once

#include <memory>
#include <string>
#include <utility>

namespace resources
{

struct data_buffer
{
    char* data;
    size_t data_size;
};

struct abstract_resource
{
    ///
    /// Get the resource identifier of this resource
    ///
    virtual const std::string& get_resource_identifier() const = 0;

    ///
    /// Get the type of the resource, should be HTML compliant since it'll be sent out
    ///
    virtual const std::string& get_resource_type() const = 0;

    ///
    /// Get a pointer to some data that represents this resource, this will be sent as a
    /// string. This function may modify data, like if fetch the resource
    ///
    virtual data_buffer get_resource() = 0;

    ///
    /// Convenience typedef
    ///
    using ptr = std::shared_ptr<abstract_resource>;
};

}
