#pragma once
#include "ypp/ast.hpp"
#include <string>
#include <vector>

namespace ypp {
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::vector<Node> parse();
    const std::vector<std::string>& errors() const noexcept { return errors_; }

private:
    const Token& peek(std::size_t lookahead = 0) const;
    bool atEnd() const;
    bool check(TokenKind kind) const;
    bool checkWord(const char* word) const;
    Token advance();
    bool match(TokenKind kind);
    bool matchWord(const char* word);
    bool consume(TokenKind kind, const char* message);
    Node declarationOrStatement();
    Node parseBlock();
    Node parseDirective();
    Node parseVariable();
    Node parseFunction();
    Node parseConditional();
    Node parseLoop();
    Node parseOutput();
    Node parseGive();
    Node parseExpressionStatement();
    std::vector<Token> collectUntil(TokenKind terminator);
    std::vector<Token> collectParenthesized();
    void synchronize();
    void error(const Token& token, const std::string& message);

    std::vector<Token> tokens_;
    std::size_t current_{0};
    std::vector<std::string> errors_;
};
}
