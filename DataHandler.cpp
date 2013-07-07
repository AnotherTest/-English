#ifndef _DATAHANDLER_GUARD
#define _DATAHANDLER_GUARD
#include "DataHandler.h"
#include <iostream>

DataHandler::DataHandler()
    : var_table(), func_table()
{
    set("newline", Variable(std::string("\n")));
    set("zero", Variable(0.0));
    set("one", Variable(1.0));
    set("two", Variable(2.0));
    set("three", Variable(3.0));
    set("four", Variable(4.0));
    set("five", Variable(5.0));
    set("six", Variable(6.0));
    set("seven", Variable(7.0));
    set("eight", Variable(8.0));
    set("nine", Variable(9.0));
    // Initialize system functions
    func_table["getInput"] = sys::get_input;
    func_table["ask"] = sys::get_input;
    func_table["Display"] = sys::display;
    func_table["Show"] = sys::display;
    func_table["Output"] = sys::display;
    func_table["Echo"] = sys::display;
    func_table["Write"] = sys::display;
    func_table["Print"] = sys::display;
    func_table["toNumber"] = sys::to_number;
    func_table["toString"] = sys::to_string;
}


void DataHandler::addVar(const std::string& name)
{
    var_table.insert(std::pair<std::string, Variable>(name, Variable()));
}

void DataHandler::addFunc(const std::string& name, const std::vector<std::string>& args)
{
    usr_func_table.insert(std::pair<std::string, Function>(name, Function(args)));
}

void DataHandler::delVar(const std::string& name)
{
    var_table.erase(name);
}

bool DataHandler::varExists(const std::string& name)
{
    return var_table.find(name) != var_table.end();
}

bool DataHandler::funcExists(const std::string& name)
{
    return func_table.find(name) != func_table.end()
     || usr_func_table.find(name) != usr_func_table.end();
}

Variable DataHandler::call(const std::string& name, arg_t& args) {
    auto it1 = func_table.find(name);
    if(it1 != func_table.end())
        return (*it1->second)(args);
    auto it2 = usr_func_table.find(name);
    if(it2 != usr_func_table.end()) {
        std::vector<std::string>& fn_args = it2->second.getArgs();
        for(size_t i = 0; i < fn_args.size(); ++i)
            set(fn_args[i], args[i]);
        Variable result = it2->second.call();
        for(const std::string& name : fn_args)
            delVar(name);
        return result;
    }
    std::cerr << "undefined function \"" << name << "\" used" << std::endl;
    return Variable();
}

Variable& DataHandler::getVar(const std::string& name)
{
    std::map<std::string, Variable>::iterator it = var_table.find(name);
    if(it == var_table.end())
        std::cerr << "undefined variable \"" << name << "\" used" << std::endl;
    return it->second;
}

Function& DataHandler::getFunc(const std::string& name)
{
    auto it = usr_func_table.find(name);
    if (it == usr_func_table.end())
        std::cerr << "undefined function \"" << name << "\" used" << std::endl;
    return it->second;
}

void DataHandler::set(const std::string& name, const Variable& value)
{
    var_table[name] = value;
}

#endif
