#include "Function.h"
#include "Ast.h"

Function::Function(const std::vector<std::string>& args)
    : args(args)
{

}

void Function::setBody(Ast::Block* b)
{
    body = b;
}

VarPtr Function::call()
{
    Ast::Value temp = body->execute();
    return temp ? temp.get<VarPtr>() : VarPtr(new Variable());
}
