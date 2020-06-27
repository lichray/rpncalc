#pragma once

#include <string>
#include <optional>
#include <unordered_map>
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
    std::unordered_map<std::string_view, decltype(ls_.begin())> tbl_;

  public:
    void set(std::string&& name, double val);
    auto operator[](std::string_view name) const -> std::optional<double>;

    friend auto begin(environment const& env) noexcept
    {
        return env.ls_.begin();
    }

    friend auto end(environment const& env) noexcept { return env.ls_.end(); }

    friend auto size(environment const& env) noexcept
    {
        return env.ls_.size();
    }
};

}
