#ifndef _DATAHANDLERH_GUARD
#define _DATAHANDLERH_GUARD
#include <string>
#include <map>
#include <deque>
#include <typeinfo>
#include "Variable.h"
#include "SysFunctions.h"
#include "Function.h"

// Useful typedef
typedef VarPtr (*SysFunc)(arg_t&);

template<class T>
VarPtr make_variable(const T& v)
{
    return VarPtr(new Variable(v));
}

class DataHandler;

/**
 * Represents a scope of the program. A ::Scope contains variales and functions.
 * All blocks have their own scope.
 */
class Scope {
    std::map<std::string, VarPtr> var_table;
    std::map<std::string, Function> usr_func_table;
public:
    Scope() = default;
    void addVar(const std::string& name);
    void addFunc(DataHandler* data, const std::string& name, const std::vector<std::string>& args);
    void delVar(const std::string& name);
    void delFunc(const std::string& name);
    bool varExists(const std::string& name);
    bool funcExists(const std::string& name);
    VarPtr call(const std::string& name, arg_t& args);
    void setRef(const std::string& name, const VarPtr& value);
    void set(const std::string& name, const VarPtr& value);
    VarPtr& getVar(const std::string& name);
    Function& getFunc(const std::string& name);
};

/**
 * Operates as the current ::Scope. It could be seen as a sort of 
 *  composite. The underlying data structure is a deque, which is essentially
 *  used as an iterable stack.
 * @see ::Scope
 */
class DataHandler {
    std::deque<Scope> scopes;
    std::map<std::string, SysFunc> func_table;
public:
    DataHandler();
    void addVar(const std::string& name);
    void addFunc(const std::string& name, const std::vector<std::string>& args);
    void delVar(const std::string& name);
    void delFunc(const std::string& name);
    bool varExists(const std::string& name);
    bool funcExists(const std::string& name);
    VarPtr call(const std::string& name, arg_t& args);
    void setRef(const std::string& name, const VarPtr& value);
    void set(const std::string& name, const VarPtr& value);
    VarPtr& getVar(const std::string& name);
    Function& getFunc(const std::string& name);
    void addScope();
    void popScope();
};
#endif
