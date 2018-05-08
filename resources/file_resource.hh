#pragma once

#include "resources/abstract_resource.hh"

namespace resources
{

class file_resource : public abstract_resource
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Load the file at the provided base + path, use the path as a URI. This will also
    /// attempt to infer the resource type.
    ///
    file_resource(const std::string& base, const std::string& path);

    ///
    ///
    ///
    virtual ~file_resource() = default;

public: ///////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    inline virtual const std::string& get_resource_identifier() const
    {
        return resource_identifier;
    }

    ///
    ///
    ///
    inline virtual const std::string& get_resource_type() const
    {
        return inferred_resource_type;
    }

    ///
    ///
    ///
    inline data_buffer get_resource() override
    {
        return {data, data_size};
    }

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Memory map the file so we don't have to copy it here
    ///
    void load_file(const std::string& file_path);

    ///
    /// Throw an error using the error number (converted to a string)
    ///
    static void throw_errno(const std::string& message);

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Store the resource identifier (which will be the path passed into the constructor)
    /// with a / at the start
    ///
    std::string resource_identifier;

    ///
    /// If the file extension is one that we know of, this will store the inferred resource
    /// type
    ///
    std::string inferred_resource_type;

    ///
    /// Store pointer to file and how big it is
    /// NOTE: we're assuming no one will be mucking with our file while we're running
    ///
    char* data;
    size_t data_size;
};
}
