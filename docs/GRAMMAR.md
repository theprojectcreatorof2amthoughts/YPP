# YPP starter grammar

This document describes the initial structural grammar accepted by the YPP front end. YPP is still experimental; this is not a claim of complete C++ compatibility.

## Source and library files

- Source translation units use `.ycpp`.
- Header libraries use `.hlypp`.
- Directives begin with `!#` and occupy the rest of their physical line. The first supported directive spellings are `!#include` and `!#inclib`. The lexer preserves directives as tokens; include resolution is not implemented yet.

## Preserved Y_ vocabulary

The front end recognizes these words as ordinary identifier tokens, not renamed aliases: `stash`, `define`, `give`, `when`, `doit`, `else`, `done`, `repeatwhile`, `execute`, `escape`, `writescrn`, `errstate`, `both`, `either`, and `invert`.

The structural parser currently assigns statement meaning to `stash`, `define`, `give`, `when`, `else`, `repeatwhile`, and `writescrn`. Other vocabulary remains available to later semantic/runtime stages. No `print`, `let`, or `if` aliases are introduced.

## Structural grammar (simplified)

```ebnf
translation-unit = { declaration | statement } ;
declaration      = directive | variable | function ;
directive        = "!#" , { any-character-except-newline } ;
variable         = "stash" , type , "(" , identifier , ")" , [ "=" , expression ] , ";" ;
function         = "define" , identifier , "(" , [ parameter-list ] , ")" , block ;
statement        = block | conditional | loop | output | give | expression-statement ;
conditional      = "when" , "(" , expression , ")" , block , [ "else" , block ] ;
loop             = "repeatwhile" , "(" , expression , ")" , block ;
output           = "writescrn" , ">^" , expression , ";" ;
give             = "give" , [ expression ] , ";" ;
block            = "{" , { declaration | statement } , "}" ;
```

Expressions are currently token-preserved rather than fully type-checked or evaluated. This allows the lexer/parser foundation to mature without falsely claiming a complete C++ expression implementation.

## Lexer behavior

The lexer tracks 1-based line and column positions; recognizes identifiers, numeric literals, strings, character literals, punctuation, operators, and directives; and skips C++-style line and block comments.

## Current limitations

- No evaluator, native code generation, or C++ ABI bridge yet.
- No type checking or overload resolution.
- No include path lookup or `.hlypp` module loader yet.
- C++ syntax is not fully parsed. Expression token sequences are retained for later compiler stages.
- Error recovery is intentionally basic.
