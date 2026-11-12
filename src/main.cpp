#include "ypp/lexer.hpp"
#include "ypp/parser.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

namespace {
void usage() {
    std::cout << "YPP 0.1.0 - Y++ front-end\n"
                 "Usage: ypp [--check|--tokens] <file.ycpp>\n";
}
}
int main(int argc, char** argv) {
    if (argc < 2) { usage(); return 2; }
    std::string mode = "--check";
    std::string path;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") { usage(); return 0; }
        if (arg == "--tokens" || arg == "--check") mode = arg;
        else path = arg;
    }
    if (path.empty()) { usage(); return 2; }
    if (path.size() < 5 || path.substr(path.size() - 5) != ".ycpp") {
        std::cerr << "error: YPP source files must use the .ycpp extension\n";
        return 2;
    }
    std::ifstream input(path, std::ios::binary);
    if (!input) { std::cerr << "error: cannot open " << path << '\n'; return 2; }
    const std::string source((std::istreambuf_iterator<char>(input)), {});
    ypp::Lexer lexer(source);
    auto tokens = lexer.scan();
    if (mode == "--tokens") {
        for (const auto& token : tokens)
            std::cout << token.location.line << ':' << token.location.column << "  "
                      << ypp::tokenKindName(token.kind) << "  " << token.lexeme << '\n';
        return 0;
    }
    ypp::Parser parser(std::move(tokens));
    const auto ast = parser.parse();
    (void)ast;
    for (const auto& error : parser.errors())
        std::cerr << path << ':' << error << '\n';
    if (!parser.errors().empty()) {
        std::cerr << "YPP: syntax check failed (" << parser.errors().size() << " issue(s))\n";
        return 1;
    }
    std::cout << "YPP: syntax structure accepted for " << path << '\n';
    return 0;
}
