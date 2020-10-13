# Faasm Toolchain ![Toolchain tests](https://github.com/faasm/faasm-toolchain/workflows/Build/badge.svg) ![License](https://img.shields.io/github/license/faasm/faasm-toolchain.svg)

This is the toolchain for building functions for
[Faasm](https://github.com/faasm/faasm).

Faasm aims to support a range of legacy applications, so requires a toolchain
capable of compiling large projects that may require threading, C++ exceptions
and dynamic linking. To do this we have to use a custom [fork of
wasi-libc](https://github.com/faasm/wasi-libc), and our own build of the
LLVM toolchain via [this Makefile](Makefile).  

We have made some small changes to the toolchain itself to support building
WebAssembly shared libraries outside of the Emscripten target. You can see these
in [this fork](https://github.com/faasm/llvm-project) through [this
diff](https://github.com/llvm/llvm-project/compare/llvmorg-10.0.1...faasm:faasm).

More detailed docs can be found in:

- [Python tooling](docs/python.md)
- [Usage](docs/usage.md)
- [Building and Releasing](docs/release.md)
- [Upgrading LLVM](docs/upgrading-llvm.md)

