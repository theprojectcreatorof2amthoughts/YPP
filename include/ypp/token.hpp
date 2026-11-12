#pragma once
#include <cstddef>
#include <string>
#include <string_view>

namespace ypp {
enum class TokenKind {
    End, Identifier, Number, String, Character, Directive,
    LeftParen, RightParen, LeftBrace, RightBrace, LeftBracket, RightBracket,
    Comma, Semicolon, Colon, Dot, Operator, Unknown
};

struct SourceLocation {
    std::size_t offset{0};
    std::size_t line{1};
    std::size_t column{1};
};

struct Token {
    TokenKind kind{TokenKind::Unknown};
    std::string lexeme;
    SourceLocation location;
};

std::string_view tokenKindName(TokenKind kind) noexcept;
}
