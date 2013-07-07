#ifndef _DATAHANDLERH_GUARD
#define _DATAHANDLERH_GUARD
#include <string>
#include <map>
#include <typeinfo>
#include "Variable.h"
#include "SysFunctions.h"
#include "Function.h"

// Useful typedef
typedef Variable (*SysFunc)(std::vector<Variable>&);

class DataHandler {
    private:
        std::map<std::string, Variable> var_table;
        std::map<std::string, SysFunc> func_table;
        std::map<std::string, Function> usr_func_table;
    public:
        DataHandler();
        void addVar(const std::string& name);
        void addFunc(const std::string& name, const std::vector<std::string>& args);
        void delVar(const std::string& name);
        bool varExists(const std::string& name);
        bool funcExists(const std::string& name);
        Variable call(const std::string& name, arg_t& args);
        void set(const std::string& name, const Variable& value);
        Variable& getVar(const std::string& name);
        Function& getFunc(const std::string& name);
};
#endif
