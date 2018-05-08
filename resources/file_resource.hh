#pragma once

#include "resources/abstract_resource.hh"

namespace resources
{

class file_resource : public abstract_resource
{
public: ///////////////////////////////////////////////////////////////////////
    ///
    /// Holds file data, let's us know if the file has been modified since the last time we accessed it
    ///
    struct file_holder
    {
        time_t last_modified;
        size_t size;
        char* data;
    };

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
    data_buffer get_resource() override;

private: //////////////////////////////////////////////////////////////////////
    ///
    /// Get the time when this file was last modified
    ///
    time_t get_last_modified_time();

    ///
    /// Get the size of the file we're interested in
    ///
    size_t get_file_size();

    ///
    /// Memory map the file so we don't have to copy it here
    ///
    void load_file();

    ///
    /// Throw an error using the error number (converted to a string)
    ///
    void throw_errno(const std::string& message);

private: //////////////////////////////////////////////////////////////////////
    ///
    ///
    ///
    const std::string full_path;

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
    /// Holds data about the file
    ///
    file_holder file;
};
}
