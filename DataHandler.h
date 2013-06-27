#ifndef _DATAHANDLERH_GUARD
#define _DATAHANDLERH_GUARD
#include <string>
#include <map>
#include <typeinfo>
#include "Variable.h"
#include "SysFunctions.h"
// Useful typedef
typedef Variable (*SysFunc)(std::vector<Variable>&);

class DataHandler {
    private:
        std::map<std::string, Variable> var_table;
        std::map<std::string, SysFunc> func_table;
    public:
        DataHandler();
        void add(const std::string& name);
        bool varExists(const std::string& name);
        bool funcExists(const std::string& name);
        Variable call(const std::string& name, arg_t& args);
        void set(const std::string& name, const Variable& value);
        Variable get(const std::string& name);
};
#endif
