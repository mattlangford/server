#include "json/parser.hh"

#include <assert.h>

void strip_whitespace()
{
    const std::string input = "\tempty   space \"not in the \n string though\"\n abc";
    const std::string expected_output = "emptyspace\"not in the \n string though\"abc";

    std::string output = json::detail::strip_whitespace(input);

    assert(output == expected_output);
}

int main()
{
    std::cout << "Running!\n";
    strip_whitespace();
    std::cout << "Passed!\n";
}
