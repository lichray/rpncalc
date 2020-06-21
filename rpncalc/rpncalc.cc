#include "rpncalc.h"

#include <charconv>

void rpn::calculator::error(std::string_view msg) const
{
    std::string s = "error: ";
    s.append(msg);
    s.push_back('\n');
    write_(s.data(), s.size());
}

void rpn::calculator::print(double val) const
{
    char buf[20];
    auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), val);
    *ptr = '\n';
    write_(buf, ptr - buf);
}

void rpn::calculator::enter(std::string_view line)
{
}
