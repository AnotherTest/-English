#include "Function.h"
#include "Ast.h"

Function::Function(const std::vector<std::string>& args)
    : args(args), body(nullptr)
{

}

void Function::setBody(Ast::Block* b)
{
    body = b;
}

VarPtr Function::call()
{
    return body->execute();
}
