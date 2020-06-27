#pragma once

namespace rpn::cclass
{

#define RPN_CCLASS_DIGIT                                                  \
    '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' \
        : case '7' : case '8' : case '9'

#define RPN_CCLASS_ID_START                                               \
    '_' : case 'a' : case 'b' : case 'c' : case 'd' : case 'e' : case 'f' \
        : case 'g' : case 'h' : case 'i' : case 'j' : case 'k' : case 'l' \
        : case 'm' : case 'n' : case 'o' : case 'p' : case 'q' : case 'r' \
        : case 's' : case 't' : case 'u' : case 'v' : case 'w' : case 'x' \
        : case 'y' : case 'z' : case 'A' : case 'B' : case 'C' : case 'D' \
        : case 'E' : case 'F' : case 'G' : case 'H' : case 'I' : case 'J' \
        : case 'K' : case 'L' : case 'M' : case 'N' : case 'O' : case 'P' \
        : case 'Q' : case 'R' : case 'S' : case 'T' : case 'U' : case 'V' \
        : case 'W' : case 'X' : case 'Y' : case 'Z'

inline constexpr auto is_digit_or_decimal_point = [](char c) {
    switch (c)
    {
    case RPN_CCLASS_DIGIT:
    case '.': return true;
    default: return false;
    }
};

inline constexpr auto is_word_character = [](char c) {
    switch (c)
    {
    case RPN_CCLASS_DIGIT:
    case RPN_CCLASS_ID_START: return true;
    default: return false;
    }
};

}
