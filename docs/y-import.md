# Importing Y_ modules from Y++

Y++ can import a Y_ source module with the opt-in directives:

```ycpp
!#yenable = 1
!#yimport "math.yc"

writescrn >^ square(9)
```

The imported file must use the `.yc` extension. Its procedure definitions are loaded into the same Y++ runtime environment, so Y++ code can call those procedures directly and receive their returned values.

## Compatibility

This is source-level interoperability, not a second runtime or a C ABI. The imported module must use syntax supported by the shared Y_/Y++ runtime grammar. Y_-specific library directives and features that do not exist in Y++ are not automatically translated.

The enable directive is required before the first import. Without it, checking and execution both report an error. Imports are resolved relative to the process working directory, just like the existing Y++ source includes.
