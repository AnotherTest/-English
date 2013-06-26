#include "TokenHandler.h"
#include <stdexcept>
#include <iostream>

int main (int argc, char const* argv[])
{
    try {
        if(argc < 2) {
            std::cerr << "please supply filename" << std::endl;
            return 2;
        }
        TokenHandler parser(argv[1]);
        Block* ast = parser.run();
        ast->execute();
    } catch(const boost::bad_any_cast& e) {
        std::cerr << "Invalid value casting." << std::endl;
        return 1;
    } catch(const std::exception& e) {
        std::cerr << "exception caught: " << e.what() << std::endl;
        return 1;
    }
}
