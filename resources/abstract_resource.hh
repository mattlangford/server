#pragma once

#include "http_server/messages.hh"

#include <memory>
#include <string>

namespace resources
{

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
    /// Fetch a string to send out in the response to the GET message. This function may
    /// modify data, like it can fetch the resource if needed
    ///
    virtual std::string get_resource() = 0;

    ///
    /// Accept a new POST request, return if it was successful.
    /// Not all resources need to implement this
    ///
    virtual bool handle_post_request(requests::POST post_request) { return false; }

    ///
    /// Convenience typedef
    ///
    using ptr = std::shared_ptr<abstract_resource>;
};

}
