#pragma once

#include <optional>

#include <Rules.hpp>
#include <Tokenizer.hpp>

struct SyntaxData {
    RuleKey rule;
    std::optional<Token> token;
};
