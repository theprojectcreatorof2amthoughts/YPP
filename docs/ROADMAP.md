# YPP capability roadmap

YPP's target is to reach and then extend the practical capabilities of Y_ while keeping YPP a C++20 implementation. This is a work plan, not a claim that the listed capabilities already exist.

## Y_ capability parity

| Y_ capability | YPP target | Current state |
|---|---|---|
| Lexer and recursive-descent parser | C++ lexer/parser with source diagnostics | Initial lexer and structural parser exist |
| Tree-walk execution | Typed C++ runtime and evaluator | Not implemented |
| Values: void, number, string, boolean, function, memory | C++ value model and managed ownership | Not implemented |
| Nested environments and closures | Lexical scopes and callable closures | Not implemented |
| Procedures and give | User-defined functions and return propagation | Parser structure only |
| when / doit / else / done | Preserve original branch vocabulary; brace syntax may coexist | Partial brace-form parser only |
| repeatwhile / execute / done, escape | Loops, break/escape propagation, iteration guard | Partial brace-form loop parser only |
| writescrn >^ and errstate <^ | Standard output and diagnostic output | writescrn parser only |
| !$name custom variables | Compatible custom-name tokenization and environment lookup | Not implemented |
| !#inptype(...) | Prompted input expression | Not implemented |
| !#inclib and !#libinc | Embedded and disk-loaded .hlypp imports | Build-time embedding only |
| Adequit-like helper modules | YPP module/import mechanism with documented extension | Not designed |
| Checked byte memory and memdesig / memread / memwrite | Bounds-checked memory values | Not implemented |
| Math, geometry, predicates, numeric conversion | Built-in library with documented behavior | Not implemented |
| String inspection and transformation | Built-in string library | Not implemented |
| Interactive REPL | Persistent environment and multiline input | Not implemented |
| Build-time library embedding | Automatic .hlypp discovery in Make and CMake | Implemented |
| Native C++ interoperability | Explicit C++ declaration/bridge layer | Not implemented |

## Implementation sequence

1. **Runtime core:** value representation, environments, expression evaluator, user functions, closures, and statement execution.
2. **Language compatibility:** complete control-flow vocabulary, custom variables, input, output/error output, and loop escape semantics.
3. **Library system:** resolve and execute embedded .hlypp modules; add disk imports, dependency tracking, cycle detection, and useful import diagnostics.
4. **Standard library:** math, strings, numeric aggregation, geometry, memory, filesystem, and system helpers.
5. **Developer experience:** persistent REPL, command-line options, source locations, diagnostics, and debugging/tracing.
6. **C++ integration:** a documented bridge for C++ types/functions, safe ownership rules, and native compilation/linking.
7. **Hardening:** portability, error handling, resource limits, performance, packaging, and documentation.

## Design rules

- YPP remains a separate project from Y_.
- .ycpp is the source extension; .hlypp is the header-library extension.
- Do not rename, remove, or replace Y_'s established vocabulary.
- Keep C++20 as the implementation baseline.
- A feature is only marked implemented after executable behavior exists; token recognition or parser acceptance alone is not runtime support.
