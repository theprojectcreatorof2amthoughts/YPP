#pragma once
#include "ypp/token.hpp"
#include <string>
#include <vector>

namespace ypp {
enum class NodeKind {
    Block, Directive, VariableDeclaration, FunctionDeclaration,
    Conditional, Loop, Output, Give, Expression, Empty
};

struct Node {
    NodeKind kind{NodeKind::Empty};
    SourceLocation location{};
    std::string label;
    std::vector<Token> expression;
    std::vector<Node> children;
    std::vector<Node> alternative;
};
}
