#ifndef _NOTENGLISH_FUNCTION_H_INCLUDE_GUARD
#define _NOTENGLISH_FUNCTION_H_INCLUDE_GUARD

#include <vector>
#include "Variable.h"

// Forward declaration
namespace Ast {
    class Block;
}

class Function {
    std::vector<std::string> args;
    Ast::Block* body;
public:
    Function(const std::vector<std::string>& args);
    void setBody(Ast::Block* b);
    Variable call();
    std::vector<std::string>& getArgs()
    {
        return args;
    }
};

#endif // _NOTENGLISH_FUNCTION_H_INCLUDE_GUARD

