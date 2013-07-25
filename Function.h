#ifndef _NOTENGLISH_FUNCTION_H_INCLUDE_GUARD
#define _NOTENGLISH_FUNCTION_H_INCLUDE_GUARD

#include <vector>
#include "Variable.h"

// Forward declaration
namespace Ast {
    class Block;
}

class DataHandler;

class Function {
    DataHandler* data;
    std::vector<std::string> args;
    Ast::Block* body;
public:
    Function(DataHandler* data, const std::vector<std::string>& args);
    void setBody(Ast::Block* b);
    VarPtr call(arg_t& arg_vals);
    std::vector<std::string>& getArgs()
    {
        return args;
    }
};

#endif // _NOTENGLISH_FUNCTION_H_INCLUDE_GUARD

