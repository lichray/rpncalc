#include "rpncalc.h"
#include "rpncclass.h"

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
    return s.substr(std::min(s.find_first_not_of(" \t"sv), s.size()));
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

static auto parse_id(std::string_view s)
    -> std::tuple<std::string, std::string_view>
{
    auto ed =
        std::find_if_not(begin(s), end(s), rpn::cclass::is_word_character);
    auto r = std::tuple(""s, ""sv);
    auto& [tok, rest] = r;
    tok.resize(std::distance(begin(s), ed));
    std::copy(begin(s), ed, begin(tok));
    rest = s.substr(tok.size());
    return r;
}

static constexpr bool next_char_is_digit_or_dot(std::string_view s)
{
    if (s.empty())
        return false;
    else
        return rpn::cclass::is_digit_or_decimal_point(s.front());
}

void rpn::calculator::enter(std::string_view line)
{
    line = skip_blank(line);
    if (line.empty())
        return;

    switch (line.front())
    {
    case 'l':
        if (auto [token, rest] = parse_id(line); token == "let"sv)
        {
            if (auto body = skip_blank(rest); body.size() != rest.size())
                let(body);
            else
                error("require spaces after 'let'");
            break;
        }
    case 's':
        if (auto [token, rest] = parse_id(line); token == "show"sv)
        {
            if (skip_blank(rest).empty())
                show();
            else
                error("unexpected token after 'show' statement");
            break;
        }
    default: eval(line);
    }
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
        case RPN_CCLASS_DIGIT:
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
        case RPN_CCLASS_ID_START: {
            auto [name, rest] = parse_id(line);
            if (auto var = env_[name]; var.has_value())
            {
                runtime_.atom(*var);
                line = rest;
            }
            else
            {
                error("undefined variable");
                return runtime_.reset();
            }
            break;
        }
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
              [&](double v) {
                  print(v);
                  env_.set("ans", v);
              },
              [&](too_few_operators) {
                  error("too few operators");
                  return runtime_.reset();
              },
          },
          runtime_.yield_value());
}

void rpn::calculator::let(std::string_view body)
{
    if (body.empty())
        return error("expect variable name after 'let'");

    auto name = ""s;
    auto rest = ""sv;

    switch (body.front())
    {
    case RPN_CCLASS_ID_START: {
        std::tie(name, rest) = parse_id(body);
        if (auto left = skip_blank(rest); not left.empty())
        {
            if (left.size() != rest.size())
            {
                rest = left;
                goto expr;
            }
            else
                error("require spaces after variable name");
        }
        else
            error("expect number after variable name");
        break;
    }
    default: error("invalid token");
    }

    return;

expr:
    switch (rest.front())
    {
    case '-':
    case RPN_CCLASS_DIGIT:
    case '.':
        if (auto [val, left] = parse_number(rest); left.size() != rest.size())
        {
            if (skip_blank(left).empty())
                env_.set(std::move(name), val);
            else
                error("unexpected token after 'let' statement");
        }
        else
            error("invalid number");
        break;
    case RPN_CCLASS_ID_START: {
        if (auto [name_to_eval, left] = parse_id(rest);
            skip_blank(left).empty())
        {
            if (auto var = env_[name_to_eval]; var.has_value())
                env_.set(std::move(name), *var);
            else
                error("undefined variable");
        }
        else
            error("unexpected token after 'let' statement");
        break;
    }
    default: error("invalid token");
    }
}

void rpn::calculator::show() const
{
}
