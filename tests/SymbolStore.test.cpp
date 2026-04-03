#include <gtest/gtest.h>

#include <SymbolStore.hpp>

class SymbolStoreTest : public ::testing::Test {};

// resolveKeyword
TEST_F(SymbolStoreTest, ResolveKeywordReturnsCodeForDeclaredKeyword) {
    SymbolStore symbols;
    size_t code = symbols.resolveKeyword("PROGRAM");
    auto token = symbols.lookup(code);
    EXPECT_EQ(token, "PROGRAM");
}

TEST_F(SymbolStoreTest, ResolveKeywordReturnsSameCodeForSameKeyword) {
    SymbolStore symbols;
    size_t first = symbols.resolveKeyword("BEGIN");
    size_t second = symbols.resolveKeyword("BEGIN");
    EXPECT_EQ(first, second);
}

TEST_F(SymbolStoreTest, ResolveKeywordThrowsForUnknownKeyword) {
    SymbolStore symbols;
    EXPECT_THROW(symbols.resolveKeyword("UNKNOWN"), std::invalid_argument);
}

// resolveLiteral
TEST_F(SymbolStoreTest, ResolveLiteralReturnsCodeForNewLiteral) {
    SymbolStore symbols;
    size_t code = symbols.resolveLiteral("42");
    EXPECT_EQ(symbols.lookup(code), "42");
}

TEST_F(SymbolStoreTest, ResolveLiteralReturnsSameCodeForSameLiteral) {
    SymbolStore symbols;
    size_t first = symbols.resolveLiteral("100");
    size_t second = symbols.resolveLiteral("100");
    EXPECT_EQ(first, second);
}

// resolveIdentifier
TEST_F(SymbolStoreTest, ResolveIdentifierReturnsCodeForNewIdentifier) {
    SymbolStore symbols;
    size_t code = symbols.resolveIdentifier("myVar");
    EXPECT_EQ(symbols.lookup(code), "myVar");
}

TEST_F(SymbolStoreTest, ResolveIdentifierReturnsSameCodeForSameIdentifier) {
    SymbolStore symbols;
    size_t first = symbols.resolveIdentifier("myVar");
    size_t second = symbols.resolveIdentifier("myVar");
    EXPECT_EQ(first, second);
}

// isKeyword
TEST_F(SymbolStoreTest, IsKeywordReturnsTrueForKeyword) {
    SymbolStore symbols;
    EXPECT_TRUE(symbols.isKeyword("BEGIN"));
}

TEST_F(SymbolStoreTest, IsKeywordReturnsFalseForNonKeyword) {
    SymbolStore symbols;
    EXPECT_FALSE(symbols.isKeyword("foo"));
}

// lookup
TEST_F(SymbolStoreTest, LookupReturnsCorrectSymbolByCode) {
    SymbolStore symbols;
    size_t code = symbols.resolveIdentifier("hello");
    EXPECT_EQ(symbols.lookup(code), "hello");
}

TEST_F(SymbolStoreTest, LookupThrowsForInvalidCode) {
    SymbolStore symbols;
    EXPECT_THROW(symbols.lookup(99999), std::out_of_range);
}

// lookupType
TEST_F(SymbolStoreTest, LookupTypeReturnsCharacterForAsciiCode) {
    SymbolStore symbols;
    EXPECT_EQ(symbols.lookupType(static_cast<size_t>('A')), SymbolType::Ascii);
}

TEST_F(SymbolStoreTest, LookupTypeReturnsKeywordForKeywordCode) {
    SymbolStore symbols;
    size_t code = symbols.resolveKeyword("PROGRAM");
    EXPECT_EQ(symbols.lookupType(code), SymbolType::Keyword);
}

TEST_F(SymbolStoreTest, LookupTypeReturnsIdentifierForIdentifierCode) {
    SymbolStore symbols;
    size_t code = symbols.resolveIdentifier("myVar");
    EXPECT_EQ(symbols.lookupType(code), SymbolType::Identifier);
}

TEST_F(SymbolStoreTest, LookupTypeReturnsLiteralForLiteralCode) {
    SymbolStore symbols;
    size_t code = symbols.resolveLiteral("42");
    EXPECT_EQ(symbols.lookupType(code), SymbolType::Literal);
}

TEST_F(SymbolStoreTest, LookupTypeThrowsForInvalidCode) {
    SymbolStore symbols;
    EXPECT_THROW(symbols.lookupType(99999), std::out_of_range);
}
