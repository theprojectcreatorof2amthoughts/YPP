#include "ypp/embedded_libraries.hpp"
#include "ypp/lexer.hpp"
#include "ypp/parser.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
namespace {
void usage() {
    std::cout << "YPP 0.2.0 - Y++ front-end\n"
                 "Usage:\n  ypp --check <file.ycpp>\n  ypp --tokens <file.ycpp>\n"
                 "  ypp --list-libs\n  ypp --show-lib <path.hlypp>\n";
}
}
int main(int argc, char** argv) {
    if (argc < 2) { usage(); return 2; }
    const std::string mode = argv[1];
    if (mode == "--help" || mode == "-h") { usage(); return 0; }
    if (mode == "--list-libs") {
        std::size_t count = 0;
        const auto* libraries = ypp::embeddedLibraries(count);
        if (count == 0) { std::cout << "No .hlypp libraries were embedded in this build.\n"; return 0; }
        for (std::size_t i = 0; i < count; ++i) std::cout << libraries[i].path << '\n';
        return 0;
    }
    if (mode == "--show-lib") {
        if (argc != 3) { usage(); return 2; }
        const auto* library = ypp::findEmbeddedLibrary(argv[2]);
        if (!library) { std::cerr << "error: embedded .hlypp library not found: " << argv[2] << '\n'; return 1; }
        std::cout << library->source;
        return 0;
    }
    if (mode != "--check" && mode != "--tokens") { usage(); return 2; }
    if (argc != 3) { usage(); return 2; }
    const std::string path = argv[2];
    if (path.size() < 5 || path.substr(path.size() - 5) != ".ycpp") {
        std::cerr << "error: YPP source files must use the .ycpp extension\n"; return 2;
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
    for (const auto& error : parser.errors()) std::cerr << path << ':' << error << '\n';
    if (!parser.errors().empty()) {
        std::cerr << "YPP: syntax check failed (" << parser.errors().size() << " issue(s))\n"; return 1;
    }
    std::cout << "YPP: syntax structure accepted for " << path << '\n';
    return 0;
}
