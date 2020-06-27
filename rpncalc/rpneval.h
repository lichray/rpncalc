#pragma once

#include <stack>
#include <variant>

namespace rpn
{

class too_few_operators
{
};
class too_few_operands
{
};

class evaluator
{
    std::stack<double> stk_;

  public:
    void atom(double val) { stk_.push(val); }
    [[nodiscard]] auto op(char)
        -> std::variant<std::monostate, too_few_operands>;
    [[nodiscard]] auto yield_value()
        -> std::variant<double, too_few_operators>;
    void reset() { stk_ = {}; }
};

}