#pragma once
#include <cstddef>
#include <string_view>

namespace ypp {
struct EmbeddedLibrary {
    std::string_view path;
    std::string_view source;
};

// Returns a stable array generated from the repository's .hlypp files.
const EmbeddedLibrary* embeddedLibraries(std::size_t& count) noexcept;
const EmbeddedLibrary* findEmbeddedLibrary(std::string_view path) noexcept;
}
