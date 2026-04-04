#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

struct SyntaxError : std::runtime_error {
    std::string message;
    size_t row;
    size_t column;

    SyntaxError(std::string message, size_t row, size_t column);

    // Error message constants
    static constexpr auto MustStartWithProgram =
        "Program must start with 'PROGRAM' keyword";
    static constexpr auto ExpectedProcedureIdentifier =
        "Expected procedure identifier after 'PROGRAM' keyword";
    static constexpr auto ExpectedSemicolon =
        "Expected ';' after procedure identifier";
    static constexpr auto ExpectedDot =
        "Expected '.' at the end of the program";
    static constexpr auto ExpectedBeginKeyword =
        "Expected 'BEGIN' keyword before statements list";
    static constexpr auto ExpectedEndKeyword =
        "Expected 'END' keyword at the end of the block";
    static constexpr auto ExpectedConstKeyword =
        "Expected 'CONST' keyword before constant declarations";
    static constexpr auto ExpectedIdentifier = "Expected identifier";
    static constexpr auto ExpectedConstantIdentifier =
        "Expected constant identifier in constant declaration";
    static constexpr auto ExpectedEquals =
        "Expected '=' in constant declaration";
    static constexpr auto ExpectedConstantSemicolon =
        "Expected ';' at the end of constant declaration";
    static constexpr auto ExpectedOpeningQuote =
        "Expected ''' (single quote) at the beginning of constant declaration";
    static constexpr auto ExpectedClosingQuote =
        "Expected ''' (single quote) at the end of constant declaration";
    static constexpr auto ExpectedUnsignedInteger =
        "Expected unsigned integer literal";
    static constexpr auto ExpectedComma = "Expected ',' in complex number";
    static constexpr auto ExpectedExp = "Expected '$EXP' in complex number";
    static constexpr auto ExpectedOpenParen =
        "Expected '(' after '$EXP' in complex number";
    static constexpr auto ExpectedCloseParen =
        "Expected ')' after exponent in complex number";
    static constexpr auto UnexpectedEndOfFile = "Unexpected end of file";
    static constexpr auto UnexpectedSymbolsAfterEndOfProgram =
        "Unexpected symbols after end of program";
};
