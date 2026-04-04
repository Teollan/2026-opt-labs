#pragma once

namespace SyntaxError {
constexpr auto MustStartWithProgram =
    "Program must start with 'PROGRAM' keyword";
constexpr auto ExpectedProcedureIdentifier =
    "Expected procedure identifier after 'PROGRAM' keyword";
constexpr auto ExpectedSemicolon = "Expected ';' after procedure identifier";
constexpr auto ExpectedDot = "Expected '.' at the end of the program";
constexpr auto ExpectedBeginKeyword =
    "Expected 'BEGIN' keyword before statements list";
constexpr auto ExpectedEndKeyword =
    "Expected 'END' keyword at the end of the block";
constexpr auto ExpectedConstKeyword =
    "Expected 'CONST' keyword before constant declarations";
constexpr auto ExpectedIdentifier = "Expected identifier";
constexpr auto ExpectedEquals = "Expected '=' in constant declaration";
constexpr auto ExpectedConstantSemicolon =
    "Expected ';' at the end of constant declaration";
constexpr auto ExpectedOpeningQuote =
    "Expected ''' (single quote) at the beginning of constant declaration";
constexpr auto ExpectedClosingQuote =
    "Expected ''' (single quote) at the end of constant declaration";
constexpr auto ExpectedUnsignedInteger = "Expected unsigned integer literal";
constexpr auto ExpectedComma = "Expected ',' in complex number";
constexpr auto ExpectedExp = "Expected '$EXP' in complex number";
constexpr auto ExpectedOpenParen =
    "Expected '(' after '$EXP' in complex number";
constexpr auto ExpectedCloseParen =
    "Expected ')' after exponent in complex number";
constexpr auto UnexpectedEndOfFile = "Unexpected end of file";
}  // namespace SyntaxError
