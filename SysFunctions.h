#ifndef _SYSFUNCTIONS_GUARD
#define _SYSFUNCTIONS_GUARD
// Files needed for the system functions
#include <vector>
#include "Variable.h"

namespace sys {
    VarPtr get_input(arg_t& args);
    VarPtr display(arg_t& args);
    VarPtr to_number(arg_t& args);
    VarPtr to_string(arg_t& args);
}
#endif // _SYSFUNCTIONS_GUARD
