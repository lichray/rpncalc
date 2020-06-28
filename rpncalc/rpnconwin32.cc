#include "rpncon.h"

#include <io.h>
#include <windows.h>
#include <wincon.h>

void rpn::console::init()
{
    if (auto fd = _fileno(out_); fd >= 0 && _isatty(fd))
    {
        auto fh = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
        GetConsoleMode(fh, &saved_mode_);
        SetConsoleMode(fh, saved_mode_ | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

void rpn::console::reset()
{
    if (auto fd = _fileno(out_); fd >= 0 && saved_mode_ != 0)
    {
        auto fh = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
        SetConsoleMode(fh, saved_mode_);
    }
}
