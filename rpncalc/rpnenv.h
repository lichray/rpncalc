#pragma once

#include <string_view>
#include <optional>

namespace rpn
{

class environment
{
  public:
    void set(std::string_view name, double val);
    auto operator[](std::string_view name) const -> std::optional<double>;
};

}
