#include "json/parser.hh"

#include <assert.h>

void test_map()
{
    std::cout << "make vector\n";
    json::vector_type d;
    d.resize(4);
    std::cout << "resized\n";
    d[0] = "element1";
    d[1].set_map();
    d[1]["map_element"] = 23.0;
    d[1]["another_map_element"] = false;
    d[2] = false;
    json::vector_type d1;
    d[3] = d1;

    json::json j;
    j.set_map();
    j["test"] = d;
    j["boolean"] = false;

    json::json parsed = json::parse(j.get_value_as_string());

    std::cout << parsed.get_value_as_string() << "\n";
}

int main()
{
    std::cout << "Running!\n";
    test_map();
    std::cout << "Passed!\n";
}
