#include "rpnenv.h"

void rpn::environment::set(std::string&& name, double val)
{
    if (auto it = tbl_.find(name); it != tbl_.end())
    {
        ls_.splice(begin(*this), ls_, it->second);
        it->second->val = val;
    }
    else
    {
        auto pos = ls_.insert(begin(*this),
                              variable{ .name = std::move(name), .val = val });
        tbl_.emplace(pos->name, pos);
    }
}

auto rpn::environment::operator[](std::string_view name) const
    -> std::optional<double>
{
    if (auto it = tbl_.find(name); it != tbl_.end())
        return it->second->val;
    else
        return std::nullopt;
}
