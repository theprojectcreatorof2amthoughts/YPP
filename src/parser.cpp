#include "ypp/parser.hpp"
#include <sstream>

namespace ypp {
Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}
const Token& Parser::peek(std::size_t n) const {
    const auto i = current_ + n;
    return tokens_[i < tokens_.size() ? i : tokens_.size() - 1];
}
bool Parser::atEnd() const { return peek().kind == TokenKind::End; }
bool Parser::check(TokenKind k) const { return peek().kind == k; }
bool Parser::checkWord(const char* w) const { return check(TokenKind::Identifier) && peek().lexeme == w; }
Token Parser::advance() { if (!atEnd()) ++current_; return tokens_[current_ - 1]; }
bool Parser::match(TokenKind k) { if (!check(k)) return false; advance(); return true; }
bool Parser::matchWord(const char* w) { if (!checkWord(w)) return false; advance(); return true; }
bool Parser::consume(TokenKind k, const char* message) {
    if (match(k)) return true;
    error(peek(), message); return false;
}
void Parser::error(const Token& t, const std::string& message) {
    std::ostringstream out; out << t.location.line << ':' << t.location.column << ": " << message;
    errors_.push_back(out.str());
}
void Parser::synchronize() {
    while (!atEnd()) {
        if (current_ > 0 && tokens_[current_ - 1].kind == TokenKind::Semicolon) return;
        if (check(TokenKind::RightBrace)) return;
        if (checkWord("stash") || checkWord("define") || checkWord("when") ||
            checkWord("repeatwhile") || checkWord("give") || checkWord("writescrn")) return;
        advance();
    }
}
std::vector<Token> Parser::collectUntil(TokenKind terminator) {
    std::vector<Token> out;
    int parens = 0, brackets = 0, braces = 0;
    while (!atEnd()) {
        const auto k = peek().kind;
        if (k == terminator && parens == 0 && brackets == 0 && braces == 0) break;
        if (k == TokenKind::RightBrace && braces == 0 && terminator != TokenKind::RightBrace) break;
        Token t = advance();
        if (t.kind == TokenKind::LeftParen) ++parens; if (t.kind == TokenKind::RightParen) --parens;
        if (t.kind == TokenKind::LeftBracket) ++brackets; if (t.kind == TokenKind::RightBracket) --brackets;
        if (t.kind == TokenKind::LeftBrace) ++braces; if (t.kind == TokenKind::RightBrace) --braces;
        out.push_back(std::move(t));
    }
    return out;
}
std::vector<Token> Parser::collectParenthesized() {
    std::vector<Token> out;
    if (!consume(TokenKind::LeftParen, "expected '('")) return out;
    int depth = 1;
    while (!atEnd() && depth > 0) {
        Token t = advance();
        if (t.kind == TokenKind::LeftParen) ++depth;
        if (t.kind == TokenKind::RightParen) --depth;
        if (depth > 0) out.push_back(std::move(t));
    }
    if (depth != 0) error(peek(), "unterminated parenthesized expression");
    return out;
}
Node Parser::parseBlock() {
    Node n; n.kind = NodeKind::Block; n.location = peek().location;
    consume(TokenKind::LeftBrace, "expected '{'");
    while (!atEnd() && !check(TokenKind::RightBrace)) {
        const auto before = current_;
        n.children.push_back(declarationOrStatement());
        if (current_ == before) advance();
    }
    consume(TokenKind::RightBrace, "expected '}' after block");
    return n;
}
Node Parser::parseDirective() {
    Node n; n.kind = NodeKind::Directive; n.location = advance().location;
    return n;
}
Node Parser::parseVariable() {
    Node n; n.kind = NodeKind::VariableDeclaration; n.location = advance().location;
    if (match(TokenKind::Identifier)) n.label = tokens_[current_ - 1].lexeme;
    else error(peek(), "expected a type after 'stash'");
    if (match(TokenKind::LeftParen)) {
        if (match(TokenKind::Identifier)) n.label += "(" + tokens_[current_ - 1].lexeme + ")";
        else error(peek(), "expected a variable name inside stash declaration");
        consume(TokenKind::RightParen, "expected ')' after stash variable name");
    } else if (match(TokenKind::Identifier)) {
        n.label += " " + tokens_[current_ - 1].lexeme;
    } else error(peek(), "expected a variable name");
    if (match(TokenKind::Operator) && tokens_[current_ - 1].lexeme == "=")
        n.expression = collectUntil(TokenKind::Semicolon);
    consume(TokenKind::Semicolon, "expected ';' after variable declaration");
    return n;
}
Node Parser::parseFunction() {
    Node n; n.kind = NodeKind::FunctionDeclaration; n.location = advance().location;
    if (match(TokenKind::Identifier)) n.label = tokens_[current_ - 1].lexeme;
    else error(peek(), "expected function name after 'define'");
    if (match(TokenKind::LeftParen)) {
        int depth = 1;
        while (!atEnd() && depth) {
            Token t = advance();
            if (t.kind == TokenKind::LeftParen) ++depth;
            if (t.kind == TokenKind::RightParen) --depth;
            if (depth) n.expression.push_back(std::move(t));
        }
    } else error(peek(), "expected '(' after function name");
    if (check(TokenKind::LeftBrace)) n.children = parseBlock().children;
    else error(peek(), "expected function body");
    return n;
}
Node Parser::parseConditional() {
    Node n; n.kind = NodeKind::Conditional; n.location = advance().location;
    n.expression = collectParenthesized();
    if (check(TokenKind::LeftBrace)) n.children = parseBlock().children;
    else error(peek(), "expected block after when condition");
    if (matchWord("else")) {
        if (check(TokenKind::LeftBrace)) n.alternative = parseBlock().children;
        else error(peek(), "expected block after else");
    }
    return n;
}
Node Parser::parseLoop() {
    Node n; n.kind = NodeKind::Loop; n.location = advance().location;
    n.expression = collectParenthesized();
    if (check(TokenKind::LeftBrace)) n.children = parseBlock().children;
    else error(peek(), "expected block after repeatwhile condition");
    return n;
}
Node Parser::parseOutput() {
    Node n; n.kind = NodeKind::Output; n.location = advance().location;
    if (match(TokenKind::Operator) && tokens_[current_ - 1].lexeme == ">^")
        n.expression = collectUntil(TokenKind::Semicolon);
    else error(peek(), "expected '>^' after writescrn");
    consume(TokenKind::Semicolon, "expected ';' after writescrn statement");
    return n;
}
Node Parser::parseGive() {
    Node n; n.kind = NodeKind::Give; n.location = advance().location;
    n.expression = collectUntil(TokenKind::Semicolon);
    consume(TokenKind::Semicolon, "expected ';' after give statement");
    return n;
}
Node Parser::parseExpressionStatement() {
    Node n; n.kind = NodeKind::Expression; n.location = peek().location;
    n.expression = collectUntil(TokenKind::Semicolon);
    if (n.expression.empty()) { error(peek(), "expected statement"); if (!atEnd()) advance(); }
    consume(TokenKind::Semicolon, "expected ';' after expression");
    return n;
}
Node Parser::declarationOrStatement() {
    if (check(TokenKind::Directive)) return parseDirective();
    if (check(TokenKind::LeftBrace)) return parseBlock();
    if (checkWord("stash")) return parseVariable();
    if (checkWord("define")) return parseFunction();
    if (checkWord("when")) return parseConditional();
    if (checkWord("repeatwhile")) return parseLoop();
    if (checkWord("writescrn")) return parseOutput();
    if (checkWord("give")) return parseGive();
    if (match(TokenKind::Semicolon)) { Node n; n.kind = NodeKind::Empty; return n; }
    return parseExpressionStatement();
}
std::vector<Node> Parser::parse() {
    std::vector<Node> nodes;
    while (!atEnd()) {
        const auto before = current_;
        nodes.push_back(declarationOrStatement());
        if (current_ == before) advance();
        if (!errors_.empty() && current_ < tokens_.size()) synchronize();
    }
    return nodes;
}
}
