# YPP

**YPP** is a C++20-powered language project that preserves Y_'s distinctive vocabulary while adding a C++ foundation and interoperability direction.

## File formats

| Extension | Purpose |
|---|---|
| .ycpp | YPP source code |
| .hlypp | YPP Header Library |

YPP is a separate project from Y_. Its vocabulary is additive: existing words such as stash, define, give, when, doit, else, done, repeatwhile, execute, escape, writescrn, errstate, both, either, and invert are not renamed or replaced with conventional aliases.

## Build

YPP supports the same simple Make-first workflow as Y_ as well as CMake. Requires GNU Make (for Make builds), a C++20 compiler, and CMake 3.20+ (for CMake builds).

~~~sh
make
./ypp --check examples/hello.ycpp
./ypp --tokens examples/hello.ycpp
./ypp --list-libs
./ypp --show-lib std/core.hlypp
~~~

CMake alternative:

~~~sh
cmake -S . -B build
cmake --build build --config Release
./build/ypp --check examples/hello.ycpp
~~~

Clean generated Make artifacts with make clean.

## Header libraries

Like Y_'s embedded .hly system, YPP automatically discovers every .hlypp file under std/ and embeds its source into the built executable. The library embedder validates file extensions, sorts paths deterministically, and rejects duplicate paths. Use ypp --list-libs to inspect the embedded library catalog and ypp --show-lib path.hlypp to inspect an embedded library's source.

The build-time embedding pipeline is implemented. Runtime execution of !#inclib / !#libinc and semantic evaluation of library contents are still being developed; embedding a library does not yet mean its declarations are executable.

## Current implementation status

The current executable is still an early front end:
- Location-aware lexer for identifiers, literals, operators, punctuation, comments, and directives.
- Structural parser for blocks, declarations, functions, conditionals, loops, output, and give.
- Syntax-check and token-inspection CLI.
- Makefile and CMake builds with automatic .hlypp discovery and embedding.

Not implemented yet: runtime evaluation, full expression/type semantics, callable user functions, REPL, external library loading, execution of embedded libraries, memory operations, complete Y_ built-ins, native code generation, and C++ ABI bindings. These are tracked in docs/ROADMAP.md. YPP should not be described as feature-equivalent to Y_ until those runtime milestones are completed.

## Starter syntax

~~~ycpp
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
~~~

See the starter grammar in docs/GRAMMAR.md for the syntax currently recognized by the parser.
