#ifndef BYTEBEATPARSER_H_
#define BYTEBEATPARSER_H_

#include "peglib.h"

using namespace peg;

template <class T>
parser generate_parse()
{
    parser parser(R"(
                  BitOr       <- BitXor '|' BitOr / BitXor
                  BitXor      <- BitAnd '^' BitXor / BitAnd
                  BitAnd      <- ShiftRight '&' BitAnd / ShiftRight
                  ShiftRight  <- ShiftLeft '>>' ShiftRight / ShiftLeft
                  ShiftLeft   <- Subtract '<<' ShiftLeft / Subtract
                  Subtract    <- Add '-' Subtract / Add
                  Add         <- Modulo '+' Add / Modulo
                  Modulo      <- Divide '%' Modulo / Divide
                  Divide      <- Multiply '/' Divide / Multiply
                  Multiply    <- Primary '*' Multiply / Primary
                  Primary     <- '(' BitOr ')' / Number
                  Number      <- < [0-9]+ >
                  %whitespace <- [ \t]*
                 )");

    assert(any_cast<bool>(parser) == true);

    parser["BitOr"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) | std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["BitXor"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) ^ std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["BitAnd"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) & std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["ShiftRight"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) >> std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["ShiftLeft"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) << std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["Subtract"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) - std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["Add"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) + std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["Modulo"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) % std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["Divide"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) / std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["Multiply"] = [](const SemanticValues &vs)
    {
        switch (vs.choice())
        {
        case 0:
            return std::any_cast<T>(vs[0]) * std::any_cast<T>(vs[1]);
        default:
            return std::any_cast<T>(vs[0]);
        }
    };

    parser["Number"] = [](const SemanticValues &vs)
    {
        return (T)vs.token_to_number<T>();
    };

    parser.enable_packrat_parsing(); // Enable packrat parsing.

    return parser;
}

#endif // BYTEBEAT_PARSER_H_
