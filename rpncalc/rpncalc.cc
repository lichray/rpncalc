#include "rpncalc.h"

#include <charconv>
#include <algorithm>
#include <tuple>
#include <limits>
#include <ciso646>

using namespace std::literals;

template<class... Ts>
struct match : Ts...
{
    using Ts::operator()...;
};

template<class... Ts>
match(Ts...) -> match<Ts...>;

void rpn::calculator::error(std::string_view msg) const
{
    auto s = "error: "s;
    s.append(msg);
    s.push_back('\n');
    write_(s.data(), s.size());
}

void rpn::calculator::print(double val) const
{
    char buf[20];
    auto [ptr, ec] = std::to_chars(std::begin(buf), std::end(buf), val);
    *ptr = '\n';
    write_(buf, ptr - buf + 1);
}

static auto skip_blank(std::string_view s) -> std::string_view
{
    s.remove_prefix(std::min(s.find_first_not_of(" \t"sv), s.size()));
    return s;
}

static auto parse_number(std::string_view s)
    -> std::tuple<double, std::string_view>
{
    double val;
    auto first = std::to_address(begin(s));
    auto [ptr, ec] = std::from_chars(first, std::to_address(end(s)), val);
    return { ec == std::errc() ? val
                               : std::numeric_limits<double>::quiet_NaN(),
             s.substr(ptr - first) };
}

static constexpr bool next_char_is_digit_or_dot(std::string_view s)
{
    if (s.empty())
        return false;
    else
        switch (s.front())
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.': return true;
        default: return false;
        }
}

void rpn::calculator::enter(std::string_view line)
{
    line = skip_blank(line);
    if (not line.empty())
        eval(line);
}

void rpn::calculator::eval(std::string_view line)
{
    for (;;)
    {
        switch (auto ch = line.front(); ch)
        {
        case '-':
            if (next_char_is_digit_or_dot(line.substr(1)))
                goto number;
        case '+':
        case '*':
        case '/':
            if (auto r = runtime_.op(ch);
                std::holds_alternative<too_few_operands>(r))
            {
                error("too few operands");
                return runtime_.reset();
            }
            line.remove_prefix(1);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
        number:
            if (auto [val, rest] = parse_number(line);
                rest.size() != line.size())
            {
                runtime_.atom(val);
                line = rest;
            }
            else
            {
                error("invalid number");
                return runtime_.reset();
            }
            break;
        default: error("invalid token"); return runtime_.reset();
        }

        if (auto rest = skip_blank(line); not rest.empty())
        {
            if (rest.size() != line.size())
                line = rest;
            else
            {
                error("require spaces between tokens");
                return runtime_.reset();
            }
        }
        else
            break;
    }

    visit(match{
              [&](double v) { print(v); },
              [&](too_few_operators) {
                  error("too few operators");
                  return runtime_.reset();
              },
          },
          runtime_.yield_value());
}
