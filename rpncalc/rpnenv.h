#pragma once

#include <string>
#include <optional>
#include <unordered_map>

namespace rpn
{

class environment
{
    std::unordered_map<std::string, double> tbl_;

  public:
    void set(std::string&& name, double val);
    auto operator[](std::string const& name) const -> std::optional<double>;
};

}
