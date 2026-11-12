# YPP capability status

YPP is a hybrid C/C++ implementation. The interpreter semantics are ported from Y_'s C runtime; C++20 provides the executable host, developer tooling, and an explicit native-function bridge.

| Capability | Status |
|---|---|
| Makefile and CMake builds | Implemented |
| Automatic .hlypp embedding | Implemented |
| Lexer and token inspection | Implemented (C++ tooling lexer) |
| Recursive-descent language parser | Implemented in C runtime |
| Tree-walk execution and expression evaluation | Implemented |
| Values and nested environments | Implemented |
| Functions, recursion, closures, give | Implemented |
| when / doit / else / done | Implemented |
| repeatwhile / execute / done, escape | Implemented |
| writescrn >^, errstate <^ | Implemented |
| !$name and braced custom names | Implemented |
| !#inptype(...) | Implemented |
| Embedded and external .hlypp imports | Implemented |
| External .yaq Adequits | Implemented |
| Checked memory operations | Implemented |
| Math, geometry, string, conversion and predicate built-ins | Implemented |
| Persistent REPL | Implemented |
| C++ host-function ABI bridge | Implemented for void/number/string/boolean |
| General C++ class/object marshalling | Not implemented |
| Automatic C++ header parsing/binding generation | Not implemented |
| Native machine-code generation for .ycpp | Not implemented |

## Remaining compiler extensions

1. Design a stable object/value ownership ABI for C++ classes, references, and containers.
2. Add opt-in C++ header binding generation with explicit ownership and exception rules.
3. Design a native compilation pipeline (typed IR, optimization, C++/LLVM backend) if native output is desired.
4. Improve source maps, diagnostics, and test coverage across Windows, Linux, and macOS.

The current implementation is intended to match Y_'s interpreter capabilities while adding a working C++ host bridge. It is not a C++ compiler and does not claim native code generation.
