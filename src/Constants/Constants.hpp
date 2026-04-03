#pragma once

#include <cstddef>

constexpr size_t ASCII_TABLE_SIZE = 256;

namespace Keyword {
constexpr auto Program = "PROGRAM";
constexpr auto Const = "CONST";
constexpr auto Begin = "BEGIN";
constexpr auto End = "END";
}  // namespace Keyword

namespace MultiDelimiter {
constexpr auto Exp = "$EXP";
}

namespace Delimiter {
constexpr auto Semicolon = ";";
constexpr auto Dot = ".";
constexpr auto Equals = "=";
constexpr auto Quote = "'";
constexpr auto Comma = ",";
constexpr auto OpenParenthesis = "(";
constexpr auto CloseParenthesis = ")";
}  // namespace Delimiter

namespace Box {
constexpr auto Horizontal = "─";
constexpr auto Vertical = "│";
constexpr auto DownAndRight = "┌";
constexpr auto DownAndLeft = "┐";
constexpr auto UpAndRight = "└";
constexpr auto UpAndLeft = "┘";
constexpr auto VerticalAndRight = "├";
constexpr auto VerticalAndLeft = "┤";
constexpr auto DownAndHorizontal = "┬";
constexpr auto UpAndHorizontal = "┴";
constexpr auto VerticalAndHorizontal = "┼";
}  // namespace Box