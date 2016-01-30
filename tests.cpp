#include <string>
#include <iostream>
#include "json.hpp"

int main()
{
    json::value i = 5001;
    json::value b = true;
    json::value s = "Hello world";

    std::cout << i.get<int>() << ' ' << b.get<bool>() << ' ' 
	      << s.get<std::string>() << std::endl;

    std::cout << json::to_string(i) << ' ' << json::to_string(b) << ' '
              << json::to_string(s) << std::endl;

    // array test 
    json::array v;
    v.push_back(i); v.push_back(b); v.push_back(s);
    std::cout << json::to_string(v) << std::endl;

    // object test
    json::object m;
    m["f1"] = i;
    m["f2"] = b;
    m["f3"] = s;
    
    std::cout << json::to_string(m) << std::endl;

    // parsing tests
    std::string is = "{'age': 49, 'childs':['Joaquín Arroyo'], 'married':false}";
    std::cout << json::to_string( json::parse(is) ) << std::endl;

    return 0;
}
