#ifndef _SYSFUNCTIONS_GUARD
#define _SYSFUNCTIONS_GUARD
// Files needed for the system functions
#include <vector>
#include "Variable.h"

// Typedefs
typedef std::vector<Variable> arg_t;
namespace sys {
    Variable get_input(arg_t& args);
    Variable display(arg_t& args);
    Variable to_number(arg_t& args);
    Variable to_string(arg_t& args);
}
#endif // _SYSFUNCTIONS_GUARD
