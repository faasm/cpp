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

# Usage

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
conventions](https://github.com/WebAssembly/tool-conventions/blob/master/DynamicLinking.md). 

## SIMD

SIMD support is switched on using the standard Clang flags, `-msimd128` and
`-munimplemented-simd128`.  

At time of writing the [Clang WASM SIMD
header](https://github.com/llvm/llvm-project/blob/master/clang/lib/Headers/wasm_simd128.h)
was not found in Clang, so we put our own copy into our wasi-libc fork
[here](https://github.com/faasm/wasi-libc/blob/master/libc-bottom-half/headers/public/wasm_simd128.h). 

Support for SIMD in Faasm relies on the underlying WASM runtimes.

We have a hacked fork of Eigen to support WebAssembly SIMD 
[here](https://github.com/faasm/eigen-git-mirror), although this is
experimental.

# Docker

The toolchain is packaged using two Docker images:

- `faasm/toolchain` - contains the custom LLVM build, not rebuilt regularly
- `faasm/sysroot` - adds the sysroot and core libraries and is rebuilt for each
  version

See the [Actions page](https://github.com/faasm/faasm-toolchain/actions) and
[Dockerfiles](docker) for more info.

## Building Docker images 

We have two tasks for building the Docker images:

- `inv container.toolchain` - rebuilds the base toolchain image
- `inv container.sysroot` - rebuilds the sysroot image

## Releasing

To create a new release:

- Update the versions in `VERSION` and the Github Actions build
- Run `inv git.tag` from the branch you want to tag
- Push and let the CI do the rest

# Building

On the rare occasion that we do need to rebuild the toolchain (e.g. when a new 
LLVM version is released), we can run through the steps below.

## Toolchain Updates

When updating the underlying LLVM version of the toolchain you'll need to create
a [new Faasm
release](https://github.com/lsds/faasm/blob/master/docs/releases.md). The steps
should be something like:

- Upgrade the LLVM submodule if necessary (see below)
- Bump up the Faasm version (as outlined in [release docs](https://github.com/lsds/faasm/blob/master/docs/releases.md))
- Rebuild the toolchain itself (see below)
- Rebuild the basic sysroot libraries (`inv libs.toolchain --clean`)
- Rebuild 3rd party libraries (Pyodide and Tensorflow, see relevant docs)
- Rebuild and upload _all_ wasm functions (all those under `funcs`)
- Set up the runtime root (see Python docs and Ansible `runtime_fs.yml` playbook)
- Make sure all the tests run
- Create a new [release](https://github.com/lsds/faasm/blob/master/docs/releases.md)
- Rebuild all Docker images for this release

## Upgrading LLVM

To upgrade the underlying LLVM version you need to update the _submodule_ in
this project. Find the commit related to tag name for the desired release in
[the LLVM repo](https://github.com/llvm/llvm-project/releases) (e.g.
`llvmorg-10.0.0`), then:

```bash
cd third-party/llvm-project
git checkout master
git fetch origin
git checkout <tag-name>
```

Once done, check the [Makefile](Makefile) for any references to LLVM 
versions and update accordingly.

You can then follow the steps below as normal.

## Building From Scratch

To build from scratch you just need to be in the `toolchain` directory, then
run:

```bash
make clean-all
make
```

This a while as it builds a lot of the LLVM toolchain from scratch.

When it finishes, check things are up to the new version with the Faasm CLI:

```bash
inv toolchain.version
```

### Rebuilding

To rebuild, there different options. The first is just rebuilding libc:

```bash
make clean-libc
make
```

Another is rebuilding all the libs, i.e. libc, libcxx, libcxxabi and
compiler-rt:

```bash
make clean-libs
make
```

The final option is to rebuild _everything_, including Clang. This only
necessary if you need to change the underlying LLVM or Clang configuration:

```bash
make clean-all
make
```

## Troubleshooting the build

If a given project fails you need to go to the build dir (e.g. for `libcxx` this
would be `third-party/llvm-project/build/libcxx`) then look at
`CMakeFiles/CMakeError.log` and `CMakeFiles/CMakeOutput.log`.

One thing to check for is the magic `libclang_rt.builtins-wasm32.tar.gz` file.
This should get built by the build process but if you need to fiddle with it you
can download it as part of the [WASI SDK
Release](https://github.com/CraneStation/wasi-sdk/releases).
