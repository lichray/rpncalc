#include "rpnenv.h"

void rpn::environment::set(std::string&& name, double val)
{
    if (auto it = tbl_.find(name); it != tbl_.end())
        *it->second = val;
    else
        tbl_.emplace(std::move(name), ls_.emplace(end(), val));
}

auto rpn::environment::operator[](std::string const& name) const
    -> std::optional<double>
{
    if (auto it = tbl_.find(name); it != tbl_.end())
        return *it->second;
    else
        return std::nullopt;
}
