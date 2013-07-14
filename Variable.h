#ifndef _VARIABLE_GUARD
#define _VARIABLE_GUARD
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include "Variant.h"

class Variable;
typedef std::shared_ptr<Variable> VarPtr;
typedef std::vector<VarPtr> arg_t;

struct Variable {
    enum class Type {
        Number, String, Boolean, Unkown
    };
    typedef double NumberType;
    typedef std::string StringType;
    typedef bool BoolType;

    Type type;
    Variable()
        : type(Type::Unkown), value()  {}

    template<typename T>
    Variable(const T& val)
        : type(determineType(val)), value(val) {}

    template<class T>
    T getValueConst() const
    {
        try {
            return boost::get<T>(value);
        } catch(...) {
            throw std::runtime_error("type violation of Variable");
        }
    }

    template<class T>
    T& getValue()
    {
        try {
            return boost::get<T>(value);
        } catch(...) {
            throw std::runtime_error("type violation of Variable");
        }
    }

    template<class T>
    void setValue(const T& val)
    {
        type = determineType(val);
        boost::get<T>(value) = val;
    }

    template<class Visitor>
    static Variable apply(const Visitor& v, const Variable& lhs, const Variable& rhs)
    {
        return boost::apply_visitor(v, lhs.value, rhs.value);
    }

    template<class Visitor>
    static Variable apply(const Visitor& v, const Variable& var)
    {
        return boost::apply_visitor(v, var.value);
    }

    VarPtr clone() const
    {
        return VarPtr(new Variable(*this));
    }
private:
    /**
     * Determines the Variable::Type of any given value.
     * @return the Variable::Type associated with \a var
     */
    template<typename T>
    Type determineType(const T& var) {
        if(typeid(var) == typeid(NumberType))
            return Type::Number;
        else if(typeid(var) == typeid(StringType))
            return Type::String;
        else if(typeid(var) == typeid(BoolType))
            return Type::Boolean;
        else {
            throw std::runtime_error("can't determine Variable type");
            return Type::Unkown;
        }
    }

    boost::variant<NumberType, StringType, BoolType>  value;
};

struct UnaryMinusVisitor : public boost::static_visitor<double> {
    template<class T>
    double operator()(const T& lhs) const {return .0;}
    double operator()(const double& d) const
    {
        return -d;
    }
};

OPERATOR_VISITOR(EqualsVisitor, ==, Variable,
    VISITOR_PART(double, ==)
    VISITOR_PART(std::string, ==)
)
OPERATOR_VISITOR(NotEqualsVisitor, !=, Variable,
    VISITOR_PART(double, !=)
    VISITOR_PART(std::string, !=)
)
OPERATOR_VISITOR(GreaterThanVisitor, >, Variable,
    VISITOR_PART(double, >)
    VISITOR_PART(std::string, >)
)

OPERATOR_VISITOR(SmallerThanVisitor, <, Variable,
    VISITOR_PART(double, <)
    VISITOR_PART(std::string, <)
)

OPERATOR_VISITOR(AndVisitor, &&, Variable,
    VISITOR_PART(bool, &&)
)

OPERATOR_VISITOR(OrVisitor, ||, Variable,
    VISITOR_PART(bool, ||)
)

OPERATOR_VISITOR(AdditionVisitor, +, Variable,
    VISITOR_PART(double, +)
    VISITOR_PART(std::string, +)
)

OPERATOR_VISITOR(SubtractionVisitor, -, Variable,
    VISITOR_PART(double, -)
)

OPERATOR_VISITOR(MultiplicationVisitor, *, Variable,
    VISITOR_PART(double, *)
)

OPERATOR_VISITOR(DivisionVisitor, /, Variable,
    VISITOR_PART(double, /)
)

#endif
