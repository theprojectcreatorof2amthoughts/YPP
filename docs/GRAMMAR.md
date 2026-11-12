# YPP language syntax

YPP keeps Y_'s original language vocabulary and tree-walk execution model. Source programs use .ycpp; Header Library YPP modules use .hlypp.

## Statements

~~~text
stash variable = expression
define function_name(parameter, ...)
    statements
done
when condition doit
    statements
else
    statements
done
repeatwhile condition execute
    statements
done
give [expression]
escape
writescrn >^ expression
errstate <^ expression
memdesig !$buffer, size
memwrite(!$buffer, index, byte)
stash value = memread(!$buffer, index)
~~~

Statements may be separated by newlines or semicolons. ## begins a line comment. Function declarations close with done; give returns from the current function. escape exits the nearest active loop.

## Values and operators

Values are void, numbers, strings, yes, no, functions, and memory buffers. void and no are falsey; other values are truthy.

Operators include +, -, *, /, %, **, //, .., ==, ~=, <, <=, >, >=, both, either, and unary invert. + concatenates two strings; .. is the explicit string concatenation operator. both and either short-circuit and return operand values.

## Custom variables and directives

- !$name accesses a custom variable. Braced custom names may contain punctuation or spaces.
- !#inptype("prompt") reads one line and returns a string; end-of-file returns void.
- !#inclib %path.hlypp% executes an embedded .hlypp library.
- !#libinc "path.hlypp" executes an external .hlypp library from disk.
- !#adeq "path.yaq" executes an external Adequit in the caller's environment.

The embedder maps a library by its filename without the .hlypp suffix. Duplicate library basenames are rejected at build time. Nested imports have a depth limit of 64.

## Built-ins

The runtime includes:
- type, tostring, tonumber, length, input
- memread, memwrite
- abs, sqrt, floor, ceil, round, pow, min, max, sin, cos, tan, log, exp, sign
- isnumber, isstring, isboolean, isvoid, isfinite, isnan
- upper, lower, contains, startswith, endswith, substring, charat, replace, repeat, reverse
- sum, average, clamp, lerp, hypot, atan2, radians, degrees

String indexing and lengths are byte-based. Memory allocation is capped at 16 MiB and loops are capped at one million iterations.

## C++ host bridge

A host C++ program includes ypp/bridge.h, registers callbacks with ypp_register_native, then starts the runtime. Native callbacks can exchange void, number, string, and boolean values. See the README for a complete example.
