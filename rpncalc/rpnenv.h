#pragma once

#include <string>
#include <optional>
#include <vector>
#include <list>

namespace rpn
{

struct variable
{
    std::string name;
    double val;
};

class environment
{
    std::list<variable> ls_;

    struct entry
    {
        std::string_view term;
        decltype(ls_.begin()) link;
    };
    std::vector<entry> idx_;

    auto lower_bound(std::string_view name) const noexcept
        -> decltype(idx_.cbegin());

  public:
    void set(std::string&& name, double val);
    auto operator[](std::string_view name) const -> std::optional<double>;

    auto begin() const noexcept { return ls_.begin(); }
    auto end() const noexcept { return ls_.end(); }

    friend auto begin(environment const& env) noexcept { return env.begin(); }
    friend auto end(environment const& env) noexcept { return env.end(); }

    friend auto size(environment const& env) noexcept
    {
        return env.ls_.size();
    }
};

}
