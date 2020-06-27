#pragma once

#include "rpneval.h"

#include <stdexcept>

auto rpn::evaluator::op(char ch)
    -> std::variant<std::monostate, too_few_operands>
{
    if (stk_.size() < 2)
        return too_few_operands();
    else
    {
        auto y = stk_.top();
        stk_.pop();
        auto x = stk_.top();
        stk_.pop();

        switch (ch)
        {
        case '+': stk_.push(x + y); break;
        case '-': stk_.push(x - y); break;
        case '*': stk_.push(x * y); break;
        case '/': stk_.push(x / y); break;
        default: throw std::invalid_argument{ "unknown operator" };
        }

        return std::monostate();
    }
}

auto rpn::evaluator::yield_value() -> std::variant<double, too_few_operators>
{
    auto ans = stk_.top();
    stk_.pop();

    if (stk_.empty())
        return ans;
    else
        return too_few_operators();
}
