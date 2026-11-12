#include "ypp/lexer.hpp"
#include <cctype>

namespace ypp {

Lexer::Lexer(std::string_view source) : source_(source) {}

bool Lexer::atEnd() const noexcept {
    return current_ >= source_.size();
}

char Lexer::peek(std::size_t lookahead) const noexcept {
    return current_ + lookahead < source_.size()
        ? source_[current_ + lookahead]
        : '\0';
}

char Lexer::advance() noexcept {
    if (atEnd()) return '\0';
    const char c = source_[current_++];
    if (c == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    return c;
}

bool Lexer::match(char expected) noexcept {
    if (atEnd() || peek() != expected) return false;
    advance();
    return true;
}

void Lexer::emit(TokenKind kind) {
    tokens_.push_back({
        kind,
        std::string(source_.substr(start_.offset, current_ - start_.offset)),
        start_
    });
}

void Lexer::skipTrivia() {
    for (;;) {
        if (peek() == ' ' || peek() == '\r' || peek() == '\t' || peek() == '\n') {
            advance();
            continue;
        }

        if (peek() == '#' && peek(1) == '#') {
            while (!atEnd() && peek() != '\n') advance();
            continue;
        }

        if (peek() == '/' && peek(1) == '/') {
            while (!atEnd() && peek() != '\n') advance();
            continue;
        }

        if (peek() == '/' && peek(1) == '*') {
            advance();
            advance();
            while (!atEnd() && !(peek() == '*' && peek(1) == '/')) advance();
            if (!atEnd()) {
                advance();
                advance();
            }
            continue;
        }

        break;
    }
}

void Lexer::scanIdentifier() {
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        advance();
    }
    emit(TokenKind::Identifier);
}

void Lexer::scanNumber() {
    while (std::isalnum(static_cast<unsigned char>(peek())) ||
           peek() == '_' || peek() == '.') {
        advance();
    }
    emit(TokenKind::Number);
}

void Lexer::scanQuoted(char quote, TokenKind kind) {
    while (!atEnd() && peek() != quote) {
        if (peek() == '\\' && peek(1) != '\0') {
            advance();
            advance();
        } else {
            advance();
        }
    }
    if (!atEnd()) advance();
    emit(kind);
}

void Lexer::scanDirective() {
    while (!atEnd() && peek() != '\n') advance();
    emit(TokenKind::Directive);
}

void Lexer::scanToken() {
    const char c = advance();

    switch (c) {
    case '(' : emit(TokenKind::LeftParen); return;
    case ')' : emit(TokenKind::RightParen); return;
    case '{' : emit(TokenKind::LeftBrace); return;
    case '}' : emit(TokenKind::RightBrace); return;
    case '[' : emit(TokenKind::LeftBracket); return;
    case ']' : emit(TokenKind::RightBracket); return;
    case ',' : emit(TokenKind::Comma); return;
    case ';' : emit(TokenKind::Semicolon); return;
    case ':' : emit(TokenKind::Colon); return;
    case '.' : emit(TokenKind::Dot); return;
    case '"': scanQuoted('"', TokenKind::String); return;
    case '\'': scanQuoted('\'', TokenKind::Character); return;

    case '!':
        if (peek() == '#') {
            advance();
            scanDirective();
            return;
        }

        if (peek() == '$') {
            advance();
            if (peek() == '{') {
                advance();
                while (!atEnd() && peek() != '}') {
                    if (peek() == '\\' && (peek(1) == '}' || peek(1) == '\\')) {
                        advance();
                    }
                    advance();
                }
                if (!match('}')) {
                    emit(TokenKind::Unknown);
                } else {
                    emit(TokenKind::Identifier);
                }
            } else if (std::isalpha(static_cast<unsigned char>(peek())) || peek() == '_') {
                while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
                    advance();
                }
                emit(TokenKind::Identifier);
            } else {
                emit(TokenKind::Unknown);
            }
            return;
        }

        while (!atEnd() &&
               std::string_view("=<>!&|+-*/%^~?").find(peek()) != std::string_view::npos) {
            advance();
        }
        emit(TokenKind::Operator);
        return;

    default:
        break;
    }

    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        scanIdentifier();
        return;
    }

    if (std::isdigit(static_cast<unsigned char>(c))) {
        scanNumber();
        return;
    }

    if (std::string_view("+-*/%=<>!&|^~?").find(c) != std::string_view::npos) {
        while (std::string_view("=<>!&|+-*/%^~?").find(peek()) != std::string_view::npos) {
            advance();
        }
        emit(TokenKind::Operator);
        return;
    }

    emit(TokenKind::Unknown);
}

std::vector<Token> Lexer::scan() {
    while (!atEnd()) {
        skipTrivia();
        if (atEnd()) break;

        start_ = {current_, line_, column_};
        scanToken();
    }

    tokens_.push_back({TokenKind::End, "", {current_, line_, column_}});
    return tokens_;
}

std::string_view tokenKindName(TokenKind kind) noexcept {
    switch (kind) {
    case TokenKind::End: return "end";
    case TokenKind::Identifier: return "identifier";
    case TokenKind::Number: return "number";
    case TokenKind::String: return "string";
    case TokenKind::Character: return "character";
    case TokenKind::Directive: return "directive";
    case TokenKind::LeftParen: return "(";
    case TokenKind::RightParen: return ")";
    case TokenKind::LeftBrace: return "{";
    case TokenKind::RightBrace: return "}";
    case TokenKind::LeftBracket: return "[";
    case TokenKind::RightBracket: return "]";
    case TokenKind::Comma: return ",";
    case TokenKind::Semicolon: return ";";
    case TokenKind::Colon: return ":";
    case TokenKind::Dot: return ".";
    case TokenKind::Operator: return "operator";
    case TokenKind::Unknown: return "unknown";
    }
    return "unknown";
}

} // namespace ypp
