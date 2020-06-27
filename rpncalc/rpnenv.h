#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include <list>

namespace rpn
{

class environment
{
    std::list<double> ls_;
    std::unordered_map<std::string, decltype(ls_.begin())> tbl_;

  public:
    void set(std::string&& name, double val);
    auto operator[](std::string const& name) const -> std::optional<double>;

    auto begin() const noexcept { return ls_.begin(); }
    auto end() const noexcept { return ls_.end(); }
};

}
