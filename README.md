# YLANG++ (YPP / Y++)

YLANG++ (formerly YPP) is a hybrid C/C++ programming language project: it carries Y_'s interpreter/runtime model forward in C and provides a C++20 host, tooling, and native-function bridge. The language keeps Y_'s vocabulary rather than replacing it with C-like aliases.

## File formats

| Extension | Purpose |
|---|---|
| .ycpp | YPP source program |
| .hlypp | Header Library YPP |
| .yaq | External Adequit helper (same executable-helper role as Y_) |

YPP is separate from Y_. It preserves stash, define, give, when, doit, else, done, repeatwhile, execute, escape, writescrn >^, errstate <^, both, either, invert, !$name, and the !# directives.

## Build

Requires a C11 compiler, a C++20 compiler, GNU Make or CMake 3.20+.

~~~sh
make
./ylang++ examples/hello.ycpp
./ypp tests/runtime.ycpp
./ypp --check examples/hello.ycpp
./ypp --list-libs
./ypp --show-lib std/math.hlypp
~~~

CMake:

~~~sh
cmake -S . -B build-cmake
cmake --build build-cmake --config Release
./build-cmake/ypp examples/hello.ycpp
~~~

Use make clean to remove Make-generated artifacts. Both build systems discover .hlypp files under std/ and embed their contents into the executable.

## Runtime capabilities

The Y_ interpreter core has been ported into YPP's C runtime and linked into a C++20 executable. It provides:
- Values: void, number, string, boolean, function, and memory.
- Nested environments, lexical closures, user-defined functions, recursion, and give.
- when / doit / else / done, repeatwhile / execute / done, and escape.
- writescrn >^ and errstate <^.
- Custom variables using !$name and braced custom names.
- Prompted input with !#inptype("prompt").
- Embedded !#inclib %path.hlypp%, external !#libinc "path.hlypp", and external !#adeq "path.yaq" imports.
- Checked byte buffers using memdesig, memread, and memwrite (up to 16 MiB).
- Y_'s math, geometry, numeric conversion, predicates, aggregation, and string built-ins.
- A persistent interactive prompt when ylang++ is run without a source file.
- Import-depth and loop-iteration limits.

The original Y_ runtime's semantics and vocabulary are retained. YPP-specific file extensions are used for source and header libraries.

## C++ interoperability

The public C-compatible bridge is include/ypp/bridge.h. A C++ host can register functions before starting the interpreter:

~~~cpp
#include "ypp/bridge.h"

extern "C" int add_native(const YPPValue *args, size_t count,
                      YPPValue *result, char *error, size_t error_capacity) {
    if (count != 2 || args[0].kind != YPP_VALUE_NUMBER ||
        args[1].kind != YPP_VALUE_NUMBER) {
        (void)error;
        (void)error_capacity;
        return 0;
    }
    result->kind = YPP_VALUE_NUMBER;
    result->number_value = args[0].number_value + args[1].number_value;
    return 1;
}

int main(int argc, char **argv) {
    ypp_register_native("add_native", add_native);
    return ypp_runtime_main(argc, argv);
}
~~~

Registered host functions are callable from YPP source like ordinary functions. Supported bridge values are void, number, string, and boolean. String results are copied by the runtime before the callback returns. The bundled CLI registers cpp_add as a working example.

This is a host-function ABI bridge, not a general C++ header parser or automatic binding generator. Native code generation and C++ class/object marshalling are future work.

## Library behavior

!#inclib %std/math.hlypp% resolves the embedded library by its filename (math) and executes it in the caller's environment. !#libinc "std/strings.hlypp" reads an external .hlypp file and executes it in the current environment. .hlypp replaces .hly for YPP; .yaq remains the external Adequit extension.

## Examples and tests

- examples/hello.ycpp demonstrates functions, conditionals, output, and an embedded library.
- tests/runtime.ycpp exercises runtime evaluation, recursion, loops, strings, memory, imports, and the C++ host bridge.
- .github/workflows/build.yml builds with both Make and CMake and runs smoke tests.

YPP is experimental. It does not claim C++ language compatibility, static type checking, a C++ parser, or native compilation of YPP programs.
