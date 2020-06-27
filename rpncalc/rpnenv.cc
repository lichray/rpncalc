#include "rpnenv.h"

void rpn::environment::set(std::string_view name, double val)
{
}

auto rpn::environment::operator[](std::string_view name) const
    -> std::optional<double>
{
    return std::nullopt;
}
