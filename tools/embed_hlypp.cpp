#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
namespace fs = std::filesystem;
static std::string normalize(const fs::path& input) {
    std::string value = input.generic_string();
    while (value.rfind("./", 0) == 0) value.erase(0, 2);
    return value;
}
static std::string quote(const std::string& value) {
    std::string out = "\"";
    const char oct[] = "01234567";
    for (unsigned char ch : value) {
        switch (ch) {
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (ch < 32 || ch >= 127) {
                out += '\\'; out += oct[(ch >> 6) & 7];
                out += oct[(ch >> 3) & 7]; out += oct[ch & 7];
            } else out += static_cast<char>(ch);
        }
    }
    out += '"';
    return out;
}
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: embed_hlypp output.cpp [library.hlypp ...]\n";
        return 2;
    }
    const fs::path output_path(argv[1]);
    std::vector<std::pair<std::string, std::string>> libraries;
    for (int i = 2; i < argc; ++i) {
        const fs::path path(argv[i]);
        if (path.extension() != ".hlypp") {
            std::cerr << argv[i] << ": expected a .hlypp header library\n"; return 1;
        }
        std::ifstream input(path, std::ios::binary);
        if (!input) { std::cerr << "cannot open library: " << argv[i] << '\n'; return 1; }
        std::string source((std::istreambuf_iterator<char>(input)), {});
        if (input.bad()) { std::cerr << "failed while reading library: " << argv[i] << '\n'; return 1; }
        const std::string name = normalize(path);
        if (std::any_of(libraries.begin(), libraries.end(), [&](const auto& item) { return item.first == name; })) {
            std::cerr << "duplicate embedded library path: " << name << '\n'; return 1;
        }
        libraries.emplace_back(name, std::move(source));
    }
    std::sort(libraries.begin(), libraries.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    std::ofstream output(output_path, std::ios::binary);
    if (!output) { std::cerr << "cannot create generated source: " << output_path.string() << '\n'; return 1; }
    output << "#include \"ypp/embedded_libraries.hpp\"\n\nnamespace ypp {\nnamespace {\nstatic const EmbeddedLibrary libraries[] = {\n";
    if (libraries.empty()) output << "    { {}, {} }\n";
    for (const auto& [path, source] : libraries)
        output << "    { " << quote(path) << ", " << quote(source) << " },\n";
    output << "};\n} // namespace\n"
              "const EmbeddedLibrary* embeddedLibraries(std::size_t& count) noexcept {\n"
              "    count = " << libraries.size() << ";\n"
              "    return libraries;\n}\n"
              "const EmbeddedLibrary* findEmbeddedLibrary(std::string_view path) noexcept {\n"
              "    std::size_t count = 0;\n"
              "    const auto* items = embeddedLibraries(count);\n"
              "    for (std::size_t i = 0; i < count; ++i)\n"
              "        if (items[i].path == path) return &items[i];\n"
              "    return nullptr;\n}\n"
              "} // namespace ypp\n";
    if (!output) { std::cerr << "failed while writing generated source\n"; return 1; }
    return 0;
}
