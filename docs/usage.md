# Usage

Everything in this repo is containerised, so to use the tooling you must do so
from within one of the containers. You can also create your own images that
extend those defined in this repo.

The [here.py](../bin/here.py) script is a convenience wrapper that will mount
your cwd into the `cpp-sysroot` container at `/work`, e.g.

```bash
# Go to some dir
cd <my work>

# Run the here script
python3 <this repo>/bin/here.py

# Now you should be in the container, so can run
/usr/local/faasm/toolchain/bin/clang --version
/usr/local/faasm/toolchain/bin/llc --version
```

## Development

To develop the project you need to go through Faasm as per [the
docs](https://github.com/faasm/faasm/blob/main/docs/development.md).

From inside the `cpp` CLI container spawned from there:

```bash
# List available tasks
inv -l

# Build tests
inv dev.cmake
inv dev.cc tests
/build/bin/tests
```

## Resolving symbols in editors/ IDEs

The wasm build outputs a `compile_commands.json` file at
`./build/func/compile_commands.json`. You can symlink this to the root of this
dir to help `clang-format` resolve symbols, i.e.

```bash
ln -s build/func/compile_commands.json .
```

## Shared libraries

For CMake projects, you should be able to add the following to your build:

```
  -DFAASM_BUILD_SHARED=ON
  -DCMAKE_TOOLCHAIN_FILE=<path to toolchain in this dir>
```

### Shared libraries internals

Building shared libraries to wasm is currently a little difficult. The only
supported target is Emscripten. This means that shared libraries may clash with
other parts of the toolchain (e.g. libc).

To build shared libraries, we make sure the following happens in the relevant
compiler and linker flags:

- Use the `wasm32-unknown-emscripten` target
- Exclude standard libraries (as these have been compiled as static libraries)
- Ensure all symbols are exported (otherwise modules with no `main` are empty)
- Prodce relocatable code (`-fPIC` only works with the Emscripten target)

We also have to add the `__wasi__` definition and remove the `__EMSCRIPTEN__`
definition, but this is done in our LLVM fork.

There is some more detail on the Emscripten-only support for `-fPIC`
[here](https://bugs.llvm.org/show_bug.cgi?id=42714).

The relevant sets of flags should all be captured in the toolchain files in this
repo.

### Dynamic Linking

Faasm implements dynamic linking according to the [WebAssembly tool
conventions](https://github.com/WebAssembly/tool-conventions/blob/main/DynamicLinking.md).

## SIMD

SIMD support is switched on using the standard Clang flags, `-msimd128` and
`-munimplemented-simd128`.

At time of writing the [Clang WASM SIMD
header](https://github.com/llvm/llvm-project/blob/main/clang/lib/Headers/wasm_simd128.h)
was not found in Clang, so we put our own copy into our wasi-libc fork
[here](https://github.com/faasm/wasi-libc/blob/main/libc-bottom-half/headers/public/wasm_simd128.h).

Support for SIMD in Faasm relies on the underlying WASM runtimes.

