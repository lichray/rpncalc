#pragma once

#include <stdio.h>

namespace rpn
{

class console
{
    FILE* out_;
    unsigned long saved_mode_ = 0;

    void init();
    void reset();

  public:
    explicit console(FILE* out) : out_(out) { init(); }
    ~console() { reset(); }

    void write(char const*, size_t) const;
};

}
