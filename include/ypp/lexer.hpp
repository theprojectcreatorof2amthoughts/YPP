#pragma once
#include "ypp/token.hpp"
#include <string_view>
#include <vector>

namespace ypp {
class Lexer {
public:
    explicit Lexer(std::string_view source);
    std::vector<Token> scan();

private:
    bool atEnd() const noexcept;
    char peek(std::size_t lookahead = 0) const noexcept;
    char advance() noexcept;
    bool match(char expected) noexcept;
    void skipTrivia();
    void scanToken();
    void scanIdentifier();
    void scanNumber();
    void scanQuoted(char quote, TokenKind kind);
    void scanDirective();
    void emit(TokenKind kind);

    std::string_view source_;
    std::size_t current_{0};
    std::size_t line_{1};
    std::size_t column_{1};
    SourceLocation start_{};
    std::vector<Token> tokens_;
};
}
