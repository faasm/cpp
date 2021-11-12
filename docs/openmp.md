# OpenMP

Debugging issues with OpenMP often requires second-guessing the generated code
before it hits our runtime implementation.

Unfortunately each `#pragma` is used to generate LLVM IR directly, based on the
code in
[`libomptarget`](https://github.com/llvm/llvm-project/tree/main/openmp/libomptarget),
so there is no intermediate C/C++ code.

Note that `#pragma`s are not included in preprocessing, so although we can see
code after preprocessing with the `-E` option in clang, the `#pragma`s are still
there.

We can however inspect the generated LLVM IR with:

```bash
clang++-10 -S -emit-llvm -fopenmp <some_source.cpp>
```

For an input of `some_source.cpp`, this outputs a file `some_source.ll` which is
human-readable and gives an indication of how and where the various `kmpc_`
calls are made.

