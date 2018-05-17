#pragma once

#include "json/json.hh"

namespace json
{

///
/// Given a string of json data, reconstruct a json object with it
///
json parse(const std::string& input);

namespace detail
{

///
/// Generate a new string, that is striped of white space (except when in other strings)
///
std::string strip_whitespace(const std::string& input);

///
/// Figure out what type is trying to be decoded, dispatch to the proper decoder
///
json parse_impl(const std::string& intput, size_t& index);

///
/// When parsing, we'll come across different tokens, they'll give a hint to the parser
/// for what to decode as
///
enum token_hint
{
    STRING, // starts with a "
    DOUBLE,
    BOOL, // true or false
    MAP, // starts with a {
    VECTOR, // starts with a [
    UNKNOWN
};
token_hint guess_token(const std::string& s, size_t index);

///
/// Extract these types starting at the given index. Most will throw if they're not happy with what they're parsing. Each will put the
/// index counter at the character AFTER the data structure
///
std::string parse_string(const std::string& input, size_t& index);
double parse_double(const std::string& input, size_t& index);
double parse_bool(const std::string& input, size_t& index);
json::map_type parse_map(const std::string& input, size_t& index);
json::vector_type parse_vector(const std::string& input, size_t& index);

}
}

