﻿#include "rpncalc.h"
#include "rpncclass.h"

#include <charconv>
#include <algorithm>
#include <vector>
#include <iterator>
#include <tuple>
#include <limits>
#include <cmath>
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
    using traits = std::numeric_limits<double>;
    double val;
    auto first = std::to_address(begin(s));
    auto [ptr, ec] = std::from_chars(first, std::to_address(end(s)), val);
    return std::tuple(ec == std::errc() ? val : traits::quiet_NaN(),
                      s.substr(ptr - first));
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
        [[fallthrough]];
    case 's':
        if (auto [token, rest] = parse_id(line); token == "show"sv)
        {
            if (skip_blank(rest).empty())
                show();
            else
                error("unexpected token after 'show' statement");
            break;
        }
        [[fallthrough]];
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
            [[fallthrough]];
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
                if (std::isfinite(val))
                {
                    runtime_.atom(val);
                    line = rest;
                }
                else
                {
                    error("unrepresentable number");
                    return runtime_.reset();
                }
            }
            else
            {
                error("invalid number");
                return runtime_.reset();
            }
            break;
        case RPN_CCLASS_ID_START:
        {
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
    case RPN_CCLASS_ID_START:
    {
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
        if (not next_char_is_digit_or_dot(rest.substr(1)))
            goto invalid_token;
        [[fallthrough]];
    case RPN_CCLASS_DIGIT:
    case '.':
        if (auto [val, left] = parse_number(rest); left.size() != rest.size())
        {
            if (skip_blank(left).empty())
            {
                if (std::isfinite(val))
                    env_.set(std::move(name), val);
                else
                    error("unrepresentable number");
            }
            else
                error("unexpected token after 'let' statement");
        }
        else
            error("invalid number");
        break;
    case RPN_CCLASS_ID_START:
    {
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
    default:
    invalid_token:
        error("invalid token");
    }
}

void rpn::calculator::show() const
{
    auto last = std::make_reverse_iterator(begin(env_));
    auto first = std::prev(last, std::min<size_t>(size(env_), 10));
    auto name_width = [](auto first, auto last) {
        auto name_less = [](auto& x, auto& y) {
            return x.name.size() < y.name.size();
        };
        return std::max_element(first, last, name_less)->name.size();
    };

    if (first == last)
        return;

    std::for_each(first, last,
                  [buf = std::vector(name_width(first, last) + 4, ' '),
                   this](variable const& v) mutable {
                      auto& [name, val] = v;
                      auto it = std::fill_n(std::next(begin(buf)),
                                            buf.size() - 4 - name.size(), ' ');
                      std::copy(begin(name), end(name), it);
                      write_(buf.data(), buf.size());
                      print(val);
                  });
}
