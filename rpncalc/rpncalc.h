#pragma once

#include <string_view>
#include <functional>

#include "rpneval.h"
#include "rpnenv.h"

namespace rpn
{

class calculator
{
    typedef void write_sig(char const*, size_t);

    std::function<write_sig> write_;
    evaluator runtime_;
    environment env_;

    void error(std::string_view msg) const;
    void print(double) const;

    void eval(std::string_view expr);

  public:
    explicit calculator(std::function<write_sig> fn) : write_(std::move(fn)) {}
    void enter(std::string_view line);
};

}
