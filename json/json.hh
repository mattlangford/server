#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

#include <iostream>

#include "variant.hh"

namespace json
{

class json
{
public:
    json() = default;

    json(const json& j) = default;

    ~json() = default;

public: //////////////////////////////////////////////////////////////////////
    using map_type = std::map<std::string, json>;
    using vector_type = std::vector<json>;

    using json_holder = variant<std::string,
                                double,
                                bool,
                                map_type,
                                vector_type>;

public: ///////////////////////////////////////////////////////////////////////
    ///
    /// We need to overload operators that are expected to exist for the types in the holder
    ///
    inline json& operator[](const std::string& key) { return holder.get<map_type>()[key]; }
    inline json& operator[](const size_t index) { return holder.get<vector_type>()[index]; }

    inline void operator=(const std::string& s) { holder.set(s); }
    inline void operator=(const double& d) { holder.set(d); }
    inline void operator=(const bool& b) { holder.set(b); }
    inline void operator=(const map_type& m) { holder.set(m); }
    inline void operator=(const vector_type& v) { holder.set(v); }

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
