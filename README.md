# YPP

**YPP** is an experimental C++-powered language that brings the expressive, distinctive Y_ vocabulary into a C++-friendly programming environment.

## File formats

| Extension | Purpose |
|---|---|
| `.ycpp` | YPP source code |
| `.hlypp` | YPP header library |

YPP is a separate language and repository from Y_. Existing Y_ vocabulary is treated as compatibility surface: YPP builds on it rather than renaming it or replacing it with conventional aliases.

## Current status

The first implementation milestone is the C++20 front-end foundation:

- A location-aware lexer with comments, strings, numbers, operators, and YPP directives.
- A structural parser for blocks, declarations, functions, conditionals, loops, return-like `give` statements, and output statements.
- A command-line checker for `.ycpp` files.
- A written starter grammar and examples.

This is an early compiler front end, not yet a complete C++ compiler or a full Y_ runtime. The parser deliberately preserves expression tokens instead of pretending to evaluate all C++ expressions. Runtime execution, C++ interop, module resolution, and the standard library are subsequent milestones.

## Build

Requires CMake 3.20+ and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build --config Release
```

Run the syntax checker:

```sh
./build/ypp --check examples/hello.ycpp
./build/ypp --tokens examples/hello.ycpp
```

On Windows, run `build\ypp.exe` (or `build\Release\ypp.exe` with multi-config generators).

## Starter syntax

```ycpp
!#include <string>
!#inclib %std/core.hlypp%

stash int(score) = 100;

define greet(string name) {
    writescrn >^ "Hello, " + name;
    give 0;
}

define main() {
    when (score >= 50) {
        greet("YPP");
    } else {
        writescrn >^ "Keep learning!";
    }
}
```

The examples define the initial direction of the language. See [docs/GRAMMAR.md](docs/GRAMMAR.md) for the currently implemented structural grammar and known limitations.
