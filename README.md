# Faasm C/C++ Support [![CPP tests](https://github.com/faasm/cpp/workflows/Tests/badge.svg?branch=main)](https://github.com/faasm/cpp/actions) [![License](https://img.shields.io/github/license/faasm/cpp.svg)](https://github.com/faasm/cpp/blob/main/LICENSE.md)

This repo contains everything needed to build C/C++ applications for
[Faasm](https://github.com/faasm/faasm):

Faasm supports a range of legacy applications, so requires a toolchain capable
of compiling large projects that may require threading, C++ exceptions and
dynamic linking. To do this we have to use a custom [fork of
wasi-libc](https://github.com/faasm/wasi-libc), and our own build of the LLVM
toolchain via [this Makefile](Makefile).

We have made some small changes to the toolchain itself to support building
WebAssembly shared libraries outside of the Emscripten target. You can see these
in [this fork](https://github.com/faasm/llvm-project) through [this
diff](https://github.com/llvm/llvm-project/compare/llvmorg-10.0.1...faasm:faasm).

The repo also contains the C/C++ definition of the [Faasm host
interface](https://faasm.readthedocs.io/en/latest/source/host_interface.html)
along with an emulator to compile and test native applications.

More detailed docs can be found in:

- [Usage and Development](docs/usage.md)
- [Releasing](docs/release.md)
- [Upgrading LLVM](docs/upgrade-llvm.md)
- [Python library](docs/python.md)
