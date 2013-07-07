#ifndef _VARIABLE_GUARD
#define _VARIABLE_GUARD
#include <string>
#include <vector>
#include <stdexcept>
#include "Variant.h"

CONDITIONAL_OPERATOR_VISITOR(EqualsVisitor, ==)
CONDITIONAL_OPERATOR_VISITOR(NotEqualsVisitor, !=)
CONDITIONAL_OPERATOR_VISITOR(GreaterThanVisitor, >)
CONDITIONAL_OPERATOR_VISITOR(SmallerThanVisitor, <)

struct AdditionVisitor : public boost::static_visitor<> {
    template<class T, class U>
    void operator()(T&, const U&) const {}
    template<class T>
    void operator()(T& lhs, const T& rhs) const
    {
        lhs += rhs;
    }
};

struct UnaryMinusVisitor : public boost::static_visitor<> {
    template<class T>
    void operator()(T& lhs) const {}
    void operator()(double& d) const
    {
        d = -d;
    }
};

ARITHMETIC_OPERATOR_VISITOR(SubtractionVisitor, -=)
ARITHMETIC_OPERATOR_VISITOR(MultiplicationVisitor, *=)
ARITHMETIC_OPERATOR_VISITOR(DivisionVisitor, /=)

struct Variable {
    enum class Type {
        Number, String, Unkown
    };
    typedef double NumberType;
    typedef std::string StringType;

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

    template<class T, class Visitor>
    static T apply(const Visitor& v, const Variable& lhs, const Variable& rhs)
    {
        return T(boost::apply_visitor(v, lhs.value, rhs.value));
    }

    /**
     * Applies a binary visitor to this class. The first argument of which can
     * be taken as a reference by the visitor's operator().
     */
    template<class Visitor>
    Variable& apply(const Visitor& v, const Variable& rhs)
    {
        boost::apply_visitor(v, value, rhs.value);
        return *this;
    }

    /**
     * Applies a unary visitor to this class.
     */
    template<class Visitor>
    Variable& apply(const Visitor& v)
    {
        boost::apply_visitor(v, value);
        return *this;
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
        else
            return Type::Unkown;
    }

    boost::variant<NumberType, StringType>  value;
};

typedef std::vector<Variable> arg_t;

#endif
