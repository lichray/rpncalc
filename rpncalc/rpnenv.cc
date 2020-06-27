#include "rpnenv.h"

void rpn::environment::set(std::string&& name, double val)
{
    tbl_.insert_or_assign(std::move(name), val);
}

auto rpn::environment::operator[](std::string const& name) const
    -> std::optional<double>
{
    if (auto it = tbl_.find(name); it != end(tbl_))
        return it->second;
    else
        return std::nullopt;
}
