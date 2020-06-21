#include <iostream>
#include <string>
#include <functional>

#include "rpncalc.h"

int main()
{
    std::string input;
    rpn::calculator calc(std::bind_front(&std::ostream::write, &std::cout));

    while (std::cout << "   ", getline(std::cin, input))
        calc.enter(input);
}
