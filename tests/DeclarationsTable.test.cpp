#include <gtest/gtest.h>

#include <Declaration.hpp>
#include <DeclarationsTable.hpp>

class DeclarationsTableTest : public ::testing::Test {
protected:
    DeclarationsTable table;

    Declaration makeConstant(
        std::string id,
        Type type = Type::ComplexInteger,
        std::optional<Value> value = std::nullopt
    ) {
        return Declaration{
            .identifier = std::move(id),
            .kind = DeclarationKind::Constant,
            .type = type,
            .value = value,
        };
    }
};

// lookup

TEST_F(DeclarationsTableTest, LookupReturnsNulloptForUnknownIdentifier) {
    EXPECT_EQ(table.lookup("x"), std::nullopt);
}

TEST_F(DeclarationsTableTest, LookupReturnsDeclaredEntry) {
    table.declare(makeConstant("x"));
    EXPECT_NE(table.lookup("x"), std::nullopt);
}

TEST_F(DeclarationsTableTest, LookupReturnsCorrectIdentifier) {
    table.declare(makeConstant("myVar"));
    EXPECT_EQ(table.lookup("myVar")->identifier, "myVar");
}

TEST_F(DeclarationsTableTest, LookupReturnsCorrectKind) {
    table.declare(makeConstant("x"));
    EXPECT_EQ(table.lookup("x")->kind, DeclarationKind::Constant);
}

TEST_F(DeclarationsTableTest, LookupReturnsCorrectType) {
    table.declare(makeConstant("x", Type::ComplexFloat));
    EXPECT_EQ(table.lookup("x")->type, Type::ComplexFloat);
}

TEST_F(DeclarationsTableTest, LookupReturnsCorrectValue) {
    table.declare(
        makeConstant("x", Type::ComplexInteger, Value{std::complex<int>{42, 0}})
    );
    ASSERT_TRUE(table.lookup("x")->value.has_value());
    EXPECT_EQ(
        std::get<std::complex<int>>(*table.lookup("x")->value),
        (std::complex<int>{42, 0})
    );
}

TEST_F(DeclarationsTableTest, LookupDoesNotFindOtherIdentifier) {
    table.declare(makeConstant("a"));
    EXPECT_EQ(table.lookup("b"), std::nullopt);
}

// declare

TEST_F(DeclarationsTableTest, DeclareAddsEntry) {
    table.declare(makeConstant("x"));
    EXPECT_NE(table.lookup("x"), std::nullopt);
}

TEST_F(DeclarationsTableTest, DeclareManyDistinctIdentifiers) {
    table.declare(makeConstant("a"));
    table.declare(makeConstant("b"));
    table.declare(makeConstant("c"));
    EXPECT_NE(table.lookup("a"), std::nullopt);
    EXPECT_NE(table.lookup("b"), std::nullopt);
    EXPECT_NE(table.lookup("c"), std::nullopt);
}

TEST_F(DeclarationsTableTest, DeclareReportsInsertionOnFirstCall) {
    auto [_, inserted] = table.declare(makeConstant("x"));
    EXPECT_TRUE(inserted);
}

TEST_F(DeclarationsTableTest, DeclareReportsDuplicate) {
    table.declare(makeConstant("x"));
    auto [existing, inserted] = table.declare(makeConstant("x"));
    EXPECT_FALSE(inserted);
    ASSERT_NE(existing, nullptr);
    EXPECT_EQ(existing->identifier, "x");
}

TEST_F(DeclarationsTableTest, DeclareDoesNotOverwriteOnDuplicate) {
    table.declare(makeConstant("x", Type::ComplexInteger));
    table.declare(makeConstant("x", Type::ComplexFloat));
    EXPECT_EQ(table.lookup("x")->type, Type::ComplexInteger);
}

// entries

TEST_F(DeclarationsTableTest, EntriesIsEmptyWhenNothingDeclared) {
    EXPECT_TRUE(table.entries().empty());
}

TEST_F(DeclarationsTableTest, EntriesReturnsAllDeclared) {
    table.declare(makeConstant("a"));
    table.declare(makeConstant("b"));
    EXPECT_EQ(table.entries().size(), 2u);
}

TEST_F(DeclarationsTableTest, EntriesAreSortedByIdentifier) {
    table.declare(makeConstant("zebra"));
    table.declare(makeConstant("alpha"));
    table.declare(makeConstant("middle"));

    auto result = table.entries();
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0].identifier, "alpha");
    EXPECT_EQ(result[1].identifier, "middle");
    EXPECT_EQ(result[2].identifier, "zebra");
}

TEST_F(DeclarationsTableTest, EntriesContainCorrectData) {
    table.declare(makeConstant(
        "x", Type::ComplexFloat, Value{std::complex<float>{3.14f, 0.0f}}
    ));
    auto result = table.entries();
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].identifier, "x");
    EXPECT_EQ(result[0].type, Type::ComplexFloat);
    ASSERT_TRUE(result[0].value.has_value());
    auto cval = std::get<std::complex<float>>(*result[0].value);
    EXPECT_FLOAT_EQ(cval.real(), 3.14f);
    EXPECT_FLOAT_EQ(cval.imag(), 0.0f);
}
