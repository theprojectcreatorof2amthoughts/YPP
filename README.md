# YLANG++ (YXX / Y++)

YLANG++ (YXX) is a hybrid C/C++ programming language project: it carries Y_'s interpreter/runtime model forward in C and provides a C++20 host, tooling, and native-function bridge. The language keeps Y_'s vocabulary rather than replacing it with C-like aliases.

## File formats

| Extension | Purpose |
|---|---|
| .ycpp | YXX source program |
| .hlypp | Header Library YXX |
| .yaq | External Adequit helper (same executable-helper role as Y_) |

YXX is separate from Y_. It preserves stash, define, give, when, doit, else, done, repeatwhile, execute, escape, writescrn >^, errstate <^, both, either, invert, !$name, and the !# directives.

## Build

Requires a C11 compiler, a C++20 compiler, GNU Make or CMake 3.20+.

~~~sh
make
./ylang++ examples/hello.ycpp
./ylang++ tests/runtime.ycpp
./ylang++ --check examples/hello.ycpp
./ylang++ --list-libs
./ylang++ --show-lib std/math.hlypp
~~~

Build the native compiler as a separate executable with `make` (or the CMake target):

~~~sh
gy++ examples/hello.ycpp -o hello-native
./hello-native
~~~

The `gy++` driver uses `clang++` by default (override with `LLVM_CLANGXX` or `--clang++ PATH`). Before invoking Clang++, it syntax-checks the entry .ycpp file and recursively checks every imported external or embedded .hlypp library. A syntax error in a library therefore stops compilation and reports the library filename and line. It then embeds the selected .ycpp entry source in a native executable and links the YXX C runtime and embedded .hlypp libraries. External imports still resolve from their runtime paths.

CMake:

~~~sh
cmake -S . -B build-cmake
cmake --build build-cmake --config Release
./build-cmake/ylang++ examples/hello.ycpp
~~~

Use make clean to remove Make-generated artifacts. Both build systems discover .hlypp files under std/ and embed their contents into the executable.

## Runtime capabilities

The Y_ interpreter core has been ported into YCPP's C runtime and linked into a C++20 executable. It provides:
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

The original Y_ runtime's semantics and vocabulary are retained. YXX-specific file extensions are used for source and header libraries.

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

Registered host functions are callable from YXX source like ordinary functions. Supported bridge values are void, number, string, and boolean. String results are copied by the runtime before the callback returns. The bundled CLI registers cpp_add as a working example.

This is a host-function ABI bridge, not a general C++ header parser or automatic binding generator. `gy++` creates a native executable by embedding the selected YXX source and linking the YXX runtime; it does not translate each YXX statement into standalone machine code. General C++ class/object marshalling remains future work.

## Library behavior

!#inclib %std/math.hlypp% resolves the embedded library by its filename (math) and executes it in the caller's environment. !#libinc "std/strings.hlypp" reads an external .hlypp file and executes it in the current environment. .hlypp replaces .hly for YXX; .yaq remains the external Adequit extension.

## Examples and tests

- examples/hello.ycpp demonstrates functions, conditionals, output, and an embedded library.
- tests/runtime.ycpp exercises runtime evaluation, recursion, loops, strings, memory, imports, and the C++ host bridge.
- .github/workflows/build.yml builds with both Make and CMake and runs smoke tests.

YXX is experimental. It does not claim C++ language compatibility, static type checking, a C++ parser, or direct YXX-to-machine-code translation.
