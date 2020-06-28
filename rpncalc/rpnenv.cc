#include "rpnenv.h"

#include <algorithm>
#include <ciso646>

auto rpn::environment::lower_bound(std::string_view name) const noexcept
    -> decltype(idx_.cbegin())
{
    return std::lower_bound(
        idx_.begin(), idx_.end(), name,
        [](entry const& a, std::string_view b) { return a.term < b; });
}

void rpn::environment::set(std::string&& name, double val)
{
    if (auto it = lower_bound(name); it != idx_.end() and it->term == name)
    {
        ls_.splice(begin(), ls_, it->link);
        it->link->val = val;
    }
    else
    {
        auto pos =
            ls_.insert(begin(), { .name = std::move(name), .val = val });
        idx_.insert(it, { .term = pos->name, .link = pos });
    }
}

auto rpn::environment::operator[](std::string_view name) const
    -> std::optional<double>
{
    if (auto it = lower_bound(name); it != idx_.end() and it->term == name)
        return it->link->val;
    else
        return std::nullopt;
}
