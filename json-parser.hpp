//==============================================================================
// Simple JSON parser
// Author: Marcelo Arroyo (marcelo.arroyo@dc.exa.unrc.edu.ar)
//==============================================================================
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

// #include <memory>
#include <map>
#include <vector>
#include <string>

namespace json {

enum value_type { null, boolean, numberi, numberd, string, map, vector };

struct value;

typedef std::map<std::string, value> object;
typedef std::vector<value> array;

template<typename T>
bool type_check(value_type t);

template<>
bool type_check<int>(value_type t);
  
template<>
bool type_check<double>(value_type t);
    
template<>
bool type_check<bool>(value_type t);

template<>
bool type_check<std::string>(value_type t);
    
template<>
bool type_check<object>(value_type t);
    
template<>
bool type_check<array>(value_type t);

struct value {

    struct holder {
        virtual ~holder() {}
    };

    template<typename T>
    struct value_holder : holder {
        value_holder(const T v) : data(v) {}
        T data;
    };

    // Constructors
    value() : t(null), h(nullptr) {}

    value(int v) 
        : t(numberi), h(std::make_shared< value_holder<int> >(v)) {}
    
    value(double v) 
        : t(numberd), h(std::make_shared< value_holder<double> >(v)) {}
    
    value(bool v) 
        : t(boolean), h(std::make_shared< value_holder<bool> >(v)) {}
    
    value(const char * v)
        : t(string), h(std::make_shared< value_holder<std::string> >(v)) {}

    value(std::string & v) 
        : t(string), h(std::make_shared< value_holder<std::string> >(v)) {}

    value(array & v) 
        : t(vector), 
          h(std::make_shared< value_holder< array > >(v)) 
    {}

    value(object & v)
        : t(map), 
          h(std::make_shared< value_holder< object > >(v)) 
    {}

    // copy constructor
    value(const value & other) : h(other.h), t(other.t) {}

    // move constructor
    value(value && other) : h(std::move(other.h)), t(other.t) {}

    // assignment operator
    value & operator=(const value & other) {
        h = other.h; t = other.t;
        return *this;
    }

    // move assignment
    value & operator=(value && other) {
        h = std::move(other.h); t = other.t;
        return *this;
    }

    value & operator=(int v) {
        t = numberi;
        h = std::make_shared< value_holder<int> >(v);
        return *this;
    }

    value & operator=(double v) {
        t = numberd;
        h = std::make_shared< value_holder<double> >(v);
        return *this;
    }

    value & operator=(bool v) {
        t = boolean;
        h = std::make_shared< value_holder<bool> >(v);
        return *this;
    }

    value & operator=(const std::string v) {
        t = string;
        h = std::make_shared< value_holder<std::string> >(v);
        return *this;
    }

    value & operator=(const char * v) {
        t = string;
        h = std::make_shared< value_holder<std::string> >(v);
        return *this;
    }

    // for object properties asignment 
    value & operator[](const std::string prop) {
        if ( t == null ) {
            object m;
            h = std::make_shared< value_holder< object > >(m);
            t = map;
        }
        object & m = get<object>();
        return m[prop];
    }

    // for array element asignment 
    value & operator[](int v) {
        if ( t != vector ) {
            array v;
            h = std::make_shared< value_holder< array > >(v);
            t = vector;
        }
        array & a = get<array>();
        return a[v];
    }

    value_type type() const { return t; }

    bool is_null()   const { return t == null; }
    bool is_int()    const { return t == numberi; }
    bool is_real()   const { return t == numberd; }
    bool is_number() const { return is_int() or is_real(); }
    bool is_string() const { return t == string; }
    bool is_object() const { return t == map; }
    bool is_array()  const { return t == vector; }
 
    // get the concrete stored value
    template<typename T>
    T & get() const {
        if ( ! type_check<T>(t) )
            throw "json::value store a " + type_desc() + " value.";
        value_holder<T>* v = static_cast< value_holder<T>* >(h.get()); 
        return v->data;
    }

    // return the textual type description
    std::string type_desc() const {
        switch (t) {
            case null:    return "null";
            case numberi: return "number (integer)";
            case numberd: return "number (real)";
            case boolean: return "boolean";
            case string:  return "string";
            case map:     return "object";
            case vector:  return "array";
        }
    }

private:

    value_type t;
    std::shared_ptr<holder> h;
};

//==============================================================================
// JSON parser
//==============================================================================
class parser {
public:

    parser(std::istream & input) 
        : in(input), line_no(0), cc('\n'), token_type(eof), parsing_error(false) 
    {
        next_token();
    }

    value parse();
    value fail(const char * msg);
    bool error() const { return parsing_error; }
    std::string get_error() const { return error_msg; }

private:
    enum token_types { delimiter, number, string, keyword, eof };

    // status
    std::istream & in;
    int line_no;
    char cc;

    // current token
    std::string token; 
    token_types token_type;

    // parsing error
    bool parsing_error;
    std::string error_msg;
    
    // parsing functions
    void next_char(); 
    void skip_blanks();
    void next_token();
    void advance(std::string s);

    value parse_object();
    value parse_array();
    value parse_basic_value();
};

std::string to_string(const value & v);
value parse(std::istream & input);
value parse(std::string & input);

} // end namespace json

#endif

