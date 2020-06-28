#include "rpncon.h"

#include <unistd.h>

void rpn::console::init()
{
    saved_mode_ = isatty(fileno(out_));
}

void rpn::console::reset()
{
}
