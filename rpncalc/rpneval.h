#pragma once

#include <stack>

namespace rpn
{

class evaluator
{
    std::stack<double> stk_;

  public:
    void atom(double val) { stk_.push(val); }
};

}