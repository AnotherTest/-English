#ifndef _DATAHANDLER_GUARD
#define _DATAHANDLER_GUARD
#include "DataHandler.h"
#include <iostream>


void Scope::addVar(const std::string& name)
{
    var_table.insert(std::make_pair(name, VarPtr(new Variable)));
}

void Scope::addFunc(DataHandler* data, const std::string& name, const std::vector<std::string>& args)
{
    usr_func_table.insert(std::pair<std::string, Function>(
        name, Function(data, args)
    ));
}

void Scope::delVar(const std::string& name)
{
    var_table.erase(name);
}

void Scope::delFunc(const std::string& name)
{
    usr_func_table.erase(name);
}

bool Scope::varExists(const std::string& name)
{
    return var_table.find(name) != var_table.end();
}

bool Scope::funcExists(const std::string& name)
{
    return usr_func_table.find(name) != usr_func_table.end();
}

VarPtr Scope::call(const std::string& name, arg_t& args) {

    auto it = usr_func_table.find(name);
    if(it != usr_func_table.end()) {
        VarPtr result = it->second.call(args);
        // for(const std::string& name : fn_args)
        //    delVar(name); // Remove arguments
        return result;
    }
    std::cerr << "undefined function \"" << name << "\" used" << std::endl;
    return VarPtr();
}

VarPtr& Scope::getVar(const std::string& name)
{
    std::map<std::string, VarPtr>::iterator it = var_table.find(name);
    if(it == var_table.end())
        std::cerr << "undefined variable \"" << name << "\" used" << std::endl;
    return it->second;
}

Function& Scope::getFunc(const std::string& name)
{
    auto it = usr_func_table.find(name);
    if (it == usr_func_table.end())
        std::cerr << "undefined function \"" << name << "\" used" << std::endl;
    return it->second;
}

void Scope::setRef(const std::string& name, const VarPtr& value)
{
    var_table[name] = value;
}

void Scope::set(const std::string& name, const VarPtr& value)
{
    *var_table[name] = *value;
}

DataHandler::DataHandler()
    : scopes()
{
    scopes.push_front(Scope());
    Scope& front = scopes.front();
    front.setRef("newline", make_variable(std::string("\n")));
    front.setRef("zero", make_variable(0.0));
    front.setRef("one", make_variable(1.0));
    front.setRef("two", make_variable(2.0));
    front.setRef("three", make_variable(3.0));
    front.setRef("four", make_variable(4.0));
    front.setRef("five", make_variable(5.0));
    front.setRef("six", make_variable(6.0));
    front.setRef("seven", make_variable(7.0));
    front.setRef("eight", make_variable(8.0));
    front.setRef("nine", make_variable(9.0));
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
    scopes.front().addVar(name);
}

void DataHandler::addFunc(const std::string& name,
        const std::vector<std::string>& args)
{
    scopes.front().addFunc(this, name, args);
}

void DataHandler::delVar(const std::string& name)
{
    scopes.front().delVar(name);
}

void DataHandler::delFunc(const std::string& name)
{
    scopes.front().delFunc(name);
}

bool DataHandler::varExists(const std::string& name)
{
    for(Scope& scope : scopes) {
        if(scope.varExists(name))
            return true;
    }
    return false;
}

bool DataHandler::funcExists(const std::string& name)
{
    if(func_table.find(name) != func_table.end())
        return true;
    for(Scope& scope : scopes) {
        if(scope.funcExists(name))
            return true;
    }
    return false;
}

VarPtr DataHandler::call(const std::string& name, arg_t& args)
{
    auto it = func_table.find(name);
    if(it != func_table.end())
        return (*it->second)(args);
    for(Scope& scope : scopes) {
        if(scope.funcExists(name))
            return scope.call(name, args);
    }
}

void DataHandler::setRef(const std::string& name, const VarPtr& value)
{
    return scopes.front().setRef(name, value);
}

void DataHandler::set(const std::string& name, const VarPtr& value)
{
    for(Scope& scope : scopes) {
        if(scope.varExists(name))
            return scope.set(name, value);
    }
}

VarPtr& DataHandler::getVar(const std::string& name)
{
    for(Scope& scope : scopes) {
        if(scope.varExists(name))
            return scope.getVar(name);
    }
}

Function& DataHandler::getFunc(const std::string& name)
{
    for(Scope& scope : scopes) {
        if(scope.funcExists(name))
            return scope.getFunc(name);
    }
}

void DataHandler::addScope()
{
    scopes.push_front(Scope());
}

void DataHandler::popScope()
{
    scopes.pop_front();
}

#endif
