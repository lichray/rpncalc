#include <iostream>
#include <string>
#include <functional>

#include "rpncalc.h"
#include "rpncon.h"

int main()
{
    std::string input;
    rpn::console con(stdout);
    rpn::calculator calc(std::bind_front(&rpn::console::write, &con));

    while (std::cout << "   ", getline(std::cin, input))
        calc.enter(input);
}
