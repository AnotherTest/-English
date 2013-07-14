/**
 * @file Variant.h Provides a number of shorthands for declaring boost::variant
 * visitor types.
 */
#ifndef _NOTENGLISH_BOOST_VARIANT_H_INCLUDE_GUARD
#define _NOTENGLISH_BOOST_VARIANT_H_INCLUDE_GUARD

#include <boost/variant.hpp>

#define OPERATOR_VISITOR(Name, Operator, Type, Funcs)       \
struct Name : public boost::static_visitor<Type> {          \
    typedef Type VarType;                                   \
    template<class T, class U>                              \
    VarType operator()(const T& lhs, const U& rhs) const    \
    {                                                       \
        throw std::runtime_error(                           \
            "invalid usage of operator"                     \
        );                                                  \
        return VarType();                                   \
    }                                                       \
    Funcs                                                   \
};

#define VISITOR_PART(Type, Operator)                        \
VarType operator()(const Type& lhs, const Type& rhs) const  \
{                                                           \
    return VarType(lhs Operator rhs);                       \
}

#endif // _NOTENGLISH_BOOST_VARIANT_H_INCLUDE_GUARD

