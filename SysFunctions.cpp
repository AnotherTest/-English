#include "SysFunctions.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace sys {
    VarPtr get_input(arg_t& args)
    {
        std::string line;
        std::getline(std::cin, line);
        return VarPtr(new Variable(line));
    }

    VarPtr display(arg_t& args)
    {
        for(auto& arg : args) {
            switch(arg->type) {
                case Variable::Type::String:
                    std::cout << arg->getValue<Variable::StringType>();
                    break;
                case Variable::Type::Number:
                    std::cout << arg->getValue<Variable::NumberType>();
                    break;
                default:
                    throw std::runtime_error("type not supported by display");
            }
        }
        std::cout.flush();
        return VarPtr(new Variable());
    }

    VarPtr to_number(arg_t& args)
    {
        return VarPtr(new Variable(boost::lexical_cast<double>(
            args[0]->getValue<std::string>()
        )));
    }

    VarPtr to_string(arg_t& args)
    {
        return VarPtr(new Variable(boost::lexical_cast<std::string>(
            args[0]->getValue<double>()
        )));
    }
}
