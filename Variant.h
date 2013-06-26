/**
 * @file Variant.h Provides a number of shorthands for declaring boost::variant
 * visitor types.
 */
#ifndef _NOTENGLISH_BOOST_VARIANT_H_INCLUDE_GUARD
#define _NOTENGLISH_BOOST_VARIANT_H_INCLUDE_GUARD

#include <boost/variant.hpp>


#define CONDITIONAL_OPERATOR_VISITOR(Name, Operator)    \
struct Name : public boost::static_visitor<bool> {      \
    template<class T, class U>                          \
    bool operator()(const T&, const U&) const           \
    {                                                   \
        return false;                                   \
    }                                                   \
    template<class T>                                   \
    bool operator()(const T& lhs, const T& rhs) const   \
    {                                                   \
        return lhs Operator rhs;                        \
    }                                                   \
};

#define ARITHMETIC_OPERATOR_VISITOR(Name, Operator)     \
struct Name : public boost::static_visitor<> {          \
    template<class T, class U>                          \
    void operator()(T&, const U&) const {}              \
    void operator()(double& lhs, const double& rhs) const \
    {                                                   \
        lhs Operator rhs;                               \
    }                                                   \
};


#endif // _NOTENGLISH_BOOST_VARIANT_H_INCLUDE_GUARD

