#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

#include <iostream>

#include "variant.hh"

#define ADD_CONSTRUCTOR_AND_OPS(type) \
public: \
    explicit json(const type& t) { holder.set(t); } \
    inline json& operator=(const type& t) { holder.set(t); return *this; }

namespace json
{

class json
{
public: //////////////////////////////////////////////////////////////////////
    using map_type = std::map<std::string, json>;
    using vector_type = std::vector<json>;

    using json_holder = variant<std::string,
                                double,
                                bool,
                                map_type,
                                vector_type>;

public: ///////////////////////////////////////////////////////////////////////
    json() = default;

    json(const json& j) = default;

    json& operator=(const json& j) = default;

    ~json() = default;


public: ///////////////////////////////////////////////////////////////////////

    ADD_CONSTRUCTOR_AND_OPS(std::string);
    ADD_CONSTRUCTOR_AND_OPS(double);
    ADD_CONSTRUCTOR_AND_OPS(bool);
    ADD_CONSTRUCTOR_AND_OPS(map_type);
    ADD_CONSTRUCTOR_AND_OPS(vector_type);

    ///
    /// We need to overload operators that are expected to exist for the types in the holder
    ///
    inline json& operator[](const std::string& key) { return holder.get<map_type>()[key]; }
    inline json& operator[](const size_t index) { return holder.get<vector_type>()[index]; }

    ///
    ///
    ///
    std::string get_value_as_string() const;

    ///
    /// Set the type so we can do operations on it
    ///
    inline void set_map() { holder.set(map_type{}); }
    inline void set_vector() { holder.set(vector_type{}); };

private: //////////////////////////////////////////////////////////////////////
    json_holder holder;
};
}
