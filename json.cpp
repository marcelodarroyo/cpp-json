#include <istream>
#include <sstream>
#include <string>
#include <iostream>
#include <cctype>
#include "json.hpp"

// comment when debugged
// #define NDEBUG
#include <cassert>

namespace json {

//==============================================================================
// type_ckeck
//==============================================================================
template<typename T>
bool type_check(value_type t) { return false; }

template<>
bool type_check<int>(value_type t) { return t == numberi; }
  
template<>
bool type_check<double>(value_type t) { return t == numberd; }
    
template<>
bool type_check<bool>(value_type t) { return t == boolean; }

template<>
bool type_check<std::string>(value_type t) { return t == string; }
    
template<>
bool type_check<object>(value_type t) { return t == map; }
    
template<>
bool type_check<array>(value_type t) { return t == vector; }
 
//==============================================================================
// parser
//==============================================================================
value parser::fail(const char * msg)
{
    parsing_error = true;
    error_msg = msg;
    return value(msg);
}

void parser::next_char() 
{
    in.get(cc);
    if ( in.fail() ) cc = 0;
}

void parser::skip_blanks()
{
    while ( cc == ' ' or cc == '\t' or cc == '\n' or cc == '\r' ) {
        if ( cc == '\n' ) ++line_no;
        next_char();
    }
}

// next token parse delimiters and basic values
void parser::next_token() 
{
    std::string delimiters = "{}[]:,";
    std::string number_charset = "0123456789.+-Ee";
    std::string keywords[] = {"null", "true", "false"};

    skip_blanks(); token.clear();
    if ( not cc ) {
        token_type = eof;
        return;
    }

    // delimiter
    if ( delimiters.find(cc) != std::string::npos ) {
        token = cc;
        token_type = delimiter;
        std::cout << "next_token: delimiter " << token << std::endl;
        next_char();
        return;
    }

    // string
    if ( cc == '"' or cc == '\'' ) {
        next_char();
        while ( cc and cc != '"' and cc != '\'' ) {
            token += cc;
            next_char();
        }
        if ( cc == '"' or cc == '\'' ) {
            token_type = string;
            std::cout << "next_token: string " << token << std::endl;
            next_char();
            return;
        }
        else
            fail("end string delimiter expected");
    }

    // number
    while ( number_charset.find(cc) != std::string::npos ) {
        token += cc; next_char();
    }
    if ( ! token.empty() ) {
        token_type = number;
        std::cout << "next_token: number " << token << std::endl;
        return;
    }

    // keywords
    for (int k=0; k<3; k++) {
        if ( cc == keywords[k][0] ) {
            token = cc;
            next_char();
            for (int p=1; p < keywords[k].length() and keywords[k][p] == cc; p++) {
                token += cc; next_char();
            }
        }
        if ( token.length() == keywords[k].length() ) {
            token_type = keyword;
            std::cout << "next_token: keyword " << token << std::endl;
            break;
        }
    }
}

void parser::advance(std::string s)
{
    next_token();
    if ( token != s )
        fail("':' expected");
    next_token();
}

value parser::parse_object()
{
    assert(token == "{");
    std::cout << "parsing object..." << std::endl;
    object o;
    while ( true ) {
        next_token();
        if ( token_type != string )
            return fail("string expected");
        std::string prop = token;

        advance(":");

        o[prop] = parse();
        
        std::cout << '\t' << prop << ':' << to_string(o[prop]) << std::endl;

        if ( token != ","  and token != "}" )
            return fail(", or } expected");
        if ( token == "}" )
            break;
    }
    next_token();
    return o;
}

value parser::parse_array()
{
    assert(token == "[");
    std::cout << "parsing array..." << std::endl;
    array v;
    while (true) {
        next_token();
        v.push_back( parse() );
        if ( token != ","  and token != "]" )
            return fail(", or ] expected");
        if ( token == "]" )
            break;
    }
    next_token();
    return v;
}

value parser::parse_basic_value()
{
    assert( token.find("{}[],: ") == std::string::npos );
    value v;
    std::cout << "parsing basic value: " << token << std::endl;
    if ( token_type == number )
        if ( token.find(".eE") != std::string::npos )
            v = std::stod(token);
        else
            v = std::stoi(token);
    else if ( token_type == string )
        v = token;
    else if ( token_type == keyword ) {
        if ( token == "true" )
            v = true;
        else if ( token == "false" )
            v = false;
    }
    else
        return fail("Not a JSON value");
    next_token();
    return v;
} 

value parser::parse()
{
    std::cout << "In parse()...\n";
    if ( token == "{" )
        return parse_object();
    if ( token == "[" )
        return parse_array();
    return parse_basic_value();
}

value parse(std::istream & input)
{
    parser p(input);
    value v = p.parse();
    if ( p.error() )
        throw "parsing error: " + p.get_error();
    return v;
}

value parse(std::string & input)
{
    std::cout << "parsing " << input << std::endl;
    std::istringstream is(input);
    return parse(is);
}

//==============================================================================
std::string to_string(const value & v)
{
    switch (v.type() ) {
        case null:
            return std::string("null");
        case numberi:
            return std::to_string(v.get<int>());
        case numberd:
            return std::to_string(v.get<double>());
        case boolean:
            return v.get<bool>()? "true": "false";
        case string:
            return '"' + v.get<std::string>() + '"';
        case map: {
            std::cout << "to_string object: " ;
            std::string r = "{";
            auto obj = v.get<object>();
            int elems = 0;
            for (auto pair : obj) {
                if ( elems++ ) 
                    r += ',';
                r += '"' + pair.first + "\":" + to_string(pair.second);
            }
            return r + '}';
        }
        case vector: {
            std::string r = "[";
            auto obj = v.get<array>();
            int elems = 0;
            for (auto elem : obj) {
                if ( elems++ ) 
                    r += ',';
                r += to_string(elem);
            }
            return r + ']';
        }
    }
}

} // end json namespace
