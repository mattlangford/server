
#include "json/json.hh"

#include <assert.h>
#include <iostream>
#include <typeinfo>


void test_json_build()
{
    json::json j;

    j.set_map();

    j["test"] = std::string("test_string");

    json::vector_type v;
    v.resize(3);

    v[0] = std::string("test1");
    v[1] = std::string("test2");
    v[2].set_map();
    auto& map = v[2];
    map["another_layer"] = 234.23;
    j["test1"] = v;

    std::cout << j.get_value_as_string() << "\n";
}


int main()
{
    std::cout << "Running!\n";
    test_json_build();
    std::cout << "Passed!\n";
}
