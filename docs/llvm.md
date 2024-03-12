# LLVM

We use LLVM to cross-compile C and C++ code to WebAssembly. Our
cross-compilation toolchain consists of a home-built LLVM suite (`clang` and
co.) targetting WebAssembly, together with a sysroot with a set of standard
libraries to link against so that we can cross-compile meaningful applications
(most notably `libc` through [`wasi-libc`](
https://github.com/faasm/wasi-libc).

To this extent, our set-up is very similar to that of [`wasi-sdk`](
https://github.com/WebAssembly/wasi-sdk).

To upgrade the underlying LLVM version you need to:

- Update the _submodule_ in this project (see below)
- Update the version in `docker/cpp-sysroot.dockerfile`
- Update the version in `faasmtools/env.py`
- Tag and push the code
- Re-build the LLVM container (will take some time): `inv docker.build -c llvm`
- Re-build the cpp-sysroot container: `inv docker.build -c cpp-sysroot`

## Updating the LLVM submodule

Find the commit related to tag name for the desired release in
[the LLVM repo](https://github.com/llvm/llvm-project/releases) (e.g.
`llvmorg-13.0.1`), then:

```bash
cd third-party/llvm-project
git checkout main
git fetch origin
git checkout faasm
git rebase <tag_name>
```

# Rebuilding LLVM

To rebuild LLVM, there a couple of options. To rebuild just libc:

```bash
inv llvm.libc --clean
```

To rebuilding all the libs, i.e. libc, libcxx, libcxxabi and
compiler-rt:

```bash
inv llvm.libs --clean
```

The final option is to rebuild _everything_, including Clang. This only
necessary if you need to change the underlying LLVM or Clang configuration:

```bash
inv llvm --clean
```

## Troubleshooting the build

If a given project fails you need to go to the build dir (e.g. for `libcxx` this
would be `third-party/llvm-project/build/libcxx`) then look at
`CMakeFiles/CMakeError.log` and `CMakeFiles/CMakeOutput.log`.

One thing to check for is the magic `libclang_rt.builtins-wasm32.tar.gz` file.
This should get built by the build process but if you need to fiddle with it you
can download it as part of the [WASI SDK
Release](https://github.com/CraneStation/wasi-sdk/releases).
