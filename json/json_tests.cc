
#include "json/json.hh"

#include <assert.h>
#include <iostream>
#include <typeinfo>


void test_json_build()
{
    json::json j;

    std::cout << "setting map\n";
    j.set_map();
    std::cout << "map set\n";

    j["test"] = std::string("test_string");
    j["test1"] = std::string("another_string");

    std::cout << "printing as string\n";
    std::cout << j.get_value_as_string() << "\n";
}


int main()
{
    std::cout << "Running!\n";
    test_json_build();
    std::cout << "Passed!\n";
}
