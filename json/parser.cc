#include "json/parser.hh"

namespace json
{

json parse(const std::string& input)
{
    size_t index = 0;
    std::string stripped = detail::strip_whitespace(input);
    std::cout << "parsing stripped string: " << stripped << "\n";
    return detail::parse_impl(std::move(stripped), index);
}

namespace detail
{

std::string strip_whitespace(const std::string& input)
{
    std::string output;
    output.reserve(input.size());

    bool in_string = false;
    for (const char c : input)
    {
        // TODO: This doesn't work with quotes that are escaped
        constexpr char STRING_DELIMETER = '"';
        if (c == STRING_DELIMETER)
            in_string = !in_string;

        //
        // Does c need to be included in the output string
        //
        bool include_character = true;

        //
        // If we're in a string, don't do whitespace removal
        // TODO: Again, no escaping characters
        //
        if (!in_string)
        {
            constexpr char NEWLINE = '\n';
            constexpr char SPACE = ' ';
            constexpr char TAB = '\t';
            switch (c)
            {
                case NEWLINE:
                case SPACE:
                case TAB:
                    include_character = false;
                    break;
                default:
                    include_character = true;
            }
        }

        if (include_character)
            output += c;
    }
    return output;
}

//
// ############################################################################
//

json parse_impl(const std::string& input, size_t& index)
{
    switch(guess_token(input, index))
    {
        case STRING:
            return json{parse_string(input, index)};
        case DOUBLE:
            return json{parse_double(input, index)};
        case BOOL:
            return json{parse_bool(input, index)};
        case MAP:
            return json{parse_map(input, index)};
        case VECTOR:
            return json{parse_vector(input, index)};
        default:
            std::string err = "Unknown token found while parsing ";
            err += input[index];
            throw std::runtime_error(err);
    }
}

//
// ############################################################################
//

token_hint guess_token(const std::string& s, size_t index)
{
    //
    // These first three are pretty easy, since they have delimiter characters to indicate the start
    //
    constexpr char STRING_HINT = '"';
    if (s[index] == STRING_HINT)
        return token_hint::STRING;

    constexpr char MAP_HINT = '{';
    if (s[index] == MAP_HINT)
        return token_hint::MAP;

    constexpr char VECTOR_HINT = '[';
    if (s[index] == VECTOR_HINT)
        return token_hint::VECTOR;

    //
    // These are a bit harder, check explicitly if it's a bool, then assume it's a double
    //
    constexpr char TRUE_HINT = 't';
    constexpr char FALSE_HINT = 'f';
    if (s[index] == TRUE_HINT || s[index] == FALSE_HINT)
        return token_hint::BOOL;

    constexpr char DECIMAL_POINT = '.';
    if (std::isdigit(s[index]) || s[index] == DECIMAL_POINT)
    {
        return token_hint::DOUBLE;
    }

    return token_hint::UNKNOWN;
}

//
// ############################################################################
//

std::string parse_string(const std::string& input, size_t& index)
{
    std::cout << "parsing string\n";
    constexpr char STRING_DELIMETER = '"';
    if (input[index++] != STRING_DELIMETER)
        throw std::runtime_error("Tried to parse a string, but the it didn't look like a string");

    const size_t start_string = index;

    size_t size = 0;
    while (input[index++] != STRING_DELIMETER)
        size++;

    return input.substr(start_string, size);
}

//
// ############################################################################
//

double parse_double(const std::string& input, size_t& index)
{
    std::cout << "parsing double\n";
    const size_t start_index = index;

    size_t size = 0;
    constexpr char DECIMAL_POINT = '.';
    while (std::isdigit(input[index]) || input[index] == DECIMAL_POINT)
    {
        size++;
        index++;
    }

    if (size == 0)
    {
        throw std::runtime_error("Tried to parse a double, but the it didn't look like a double");
    }

    double result = std::stod(input.substr(start_index, index));
    std::cout << " - " << result << "\n";
    return result;
}

//
// ############################################################################
//

double parse_bool(const std::string& input, size_t& index)
{
    std::cout << "parsing bool\n";
    if (input.compare(index, 4, "true") == 0)
    {
        index += 4;
        std::cout << " - true\n";
        return 1.0;
    }

    if (input.compare(index, 5, "false") == 0)
    {
        index += 5;
        std::cout << " - false\n";
        return 0.0;
    }

    throw std::runtime_error("Asked to parse a bool, but no bool could be found");
}

//
// ############################################################################
//

map_type parse_map(const std::string& input, size_t& index)
{
    std::cout << "parsing map\n";
    constexpr char MAP_START_DELIMIETER = '{';
    constexpr char MAP_END_DELIMIETER = '}';
    if (input[index++] != MAP_START_DELIMIETER)
        throw std::runtime_error("Asked to parse map, but given index didn't look like a map.");

    map_type map;
    while (input[index] != MAP_END_DELIMIETER)
    {
        const std::string key = parse_string(input, index);
        std::cout << "Key is: '" << key << "'\n";

        constexpr char KEY_VALUE_SEPERATOR = ':';
        if (input[index++] != KEY_VALUE_SEPERATOR)
            throw std::runtime_error("The character after the key to a map is not a colon.");

        map[key] = parse_impl(input, index);

        constexpr char COMMA_SEPERATOR = ',';
        if (input[index] == COMMA_SEPERATOR)
            index++;
    }
    index++;

    return map;
}

//
// ############################################################################
//

vector_type parse_vector(const std::string& input, size_t& index)
{
    std::cout << "parsing vector\n";
    constexpr char VEC_START_DELIMIETER = '[';
    constexpr char VEC_END_DELIMIETER = ']';
    if (input[index++] != VEC_START_DELIMIETER)
        throw std::runtime_error("Asked to parse vector, but given index didn't look like a vector.");

    vector_type vec;
    while (input[index] != VEC_END_DELIMIETER)
    {
        std::cout << input[index] << " size: " << vec.size() << "\n";
        vec.emplace_back(parse_impl(input, index));
        std::cout << input[index] << " size: " << vec.size() << "\n";

        constexpr char COMMA_SEPERATOR = ',';
        if (input[index] == COMMA_SEPERATOR)
            index++;
    }
    index++;

    return vec;
}

}
}
