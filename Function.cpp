#include "Function.h"
#include "Ast.h"

Function::Function(DataHandler* data, const std::vector<std::string>& args)
    : data(data), args(args), body(nullptr)
{

}

void Function::setBody(Ast::Block* b)
{
    body = b;
}

VarPtr Function::call(arg_t& arg_vals)
{
    body->premakeScope();
    for(size_t i = 0; i < args.size(); ++i) {
        data->setRef(args[i], arg_vals[i]);
    }
    return body->execute();
}
