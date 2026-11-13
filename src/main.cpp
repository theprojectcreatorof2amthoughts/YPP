#include "runtime.h"
#include "embedded_libraries.h"
#include "ypp/lexer.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
extern "C" int cpp_add(const YPPValue *args, size_t count, YPPValue *result, char *error, size_t capacity) {
    if (count != 2 || args[0].kind != YPP_VALUE_NUMBER || args[1].kind != YPP_VALUE_NUMBER) {
        std::snprintf(error, capacity, "cpp_add expects exactly two numbers"); return 0;
    }
    result->kind = YPP_VALUE_NUMBER;
    result->number_value = args[0].number_value + args[1].number_value;
    return 1;
}
std::string library_key(const char *argument) {
    std::string path(argument);
    const auto slash = path.find_last_of("/\\");
    if (slash != std::string::npos) path.erase(0, slash + 1);
    const std::string suffix = ".hlypp";
    if (path.size() >= suffix.size() && path.compare(path.size() - suffix.size(), suffix.size(), suffix) == 0)
        path.erase(path.size() - suffix.size());
    return path;
}
namespace {
void usage() {
    std::cout << "YLANG++ 0.2.0 - Y++ hybrid C/C++ language runtime\n"
                 "Usage:\n  ylang++ [script.ycpp]             Run a program (no file starts the REPL)\n"
                 "  ylang++ --check <file.ycpp>       Parse without executing\n"
                 "  ylang++ --tokens <file.ycpp>      Display lexical tokens\n"
                 "  ylang++ --list-libs               List embedded .hlypp libraries\n"
                 "  ylang++ --show-lib <path.hlypp>   Display an embedded library\n"
                 "  ylang++ --version                 Show version\n";
}
}
int main(int argc, char **argv) {
    ypp_register_native("cpp_add", cpp_add);
    if (argc == 2 && std::string(argv[1]) == "--version") { std::cout << "YLANG++ 0.2.0\n"; return 0; }
    if (argc == 2 && std::string(argv[1]) == "--list-libs") {
        for (size_t i = 0; i < y_embedded_library_count(); ++i) std::cout << y_embedded_library_name(i) << '\n';
        return 0;
    }
    if (argc == 3 && std::string(argv[1]) == "--show-lib") {
        const auto key = library_key(argv[2]);
        const auto *source = y_embedded_library(key.c_str());
        if (!source) { std::cerr << "YLANG++: embedded library not found: " << key << '\n'; return 1; }
        std::cout << source; return 0;
    }
    if (argc == 3 && std::string(argv[1]) == "--check") return ypp_runtime_check_file(argv[2]);
    if (argc == 3 && std::string(argv[1]) == "--tokens") {
        const std::string path = argv[2];
        if (path.size() <= 5 || path.substr(path.size() - 5) != ".ycpp") {
            std::cerr << "YLANG++: source files must use .ycpp\n"; return 2;
        }
        std::ifstream input(path, std::ios::binary);
        if (!input) { std::cerr << "YLANG++: cannot open " << path << '\n'; return 2; }
        const std::string source((std::istreambuf_iterator<char>(input)), {});
        ypp::Lexer lexer(source);
        for (const auto &token : lexer.scan())
            std::cout << token.location.line << ':' << token.location.column << "  "
                      << ypp::tokenKindName(token.kind) << "  " << token.lexeme << '\n';
        return 0;
    }
    if (argc > 2) { usage(); return 2; }
    if (argc == 2) {
        const std::string path = argv[1];
        if (path.size() <= 5 || path.substr(path.size() - 5) != ".ycpp") {
            std::cerr << "YLANG++: source files must use the .ycpp extension\n"; return 2;
        }
    }
    return ypp_runtime_main(argc, argv);
}
