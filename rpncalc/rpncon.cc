#include "rpncon.h"

#include <string_view>
#include <ciso646>

using namespace std::literals;

void rpn::console::write(char const* p, size_t sz) const
{
    auto writes = [fp = out_](std::string_view s) {
        ::fwrite(s.data(), 1, s.size(), fp);
    };

    auto s = std::string_view(p, sz);

    if (saved_mode_ != 0)
    {
        if (constexpr auto error = "error: "sv; s.starts_with(error))
        {
            writes("\x1b[7m"sv);  // reverse background
            s.remove_prefix(error.size());
        }
        else
            writes("\x1b[1m"sv);  // highlight text
        writes(s);
        writes("\x1b[0m"sv);
    }
    else
    {
        writes(s);
    }
}
