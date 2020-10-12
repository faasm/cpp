# Upgrading LLVM

To upgrade the underlying LLVM version you need to update the _submodule_ in
this project. Find the commit related to tag name for the desired release in
[the LLVM repo](https://github.com/llvm/llvm-project/releases) (e.g.
`llvmorg-10.0.1`), then:

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

To build from scratch:

```bash
make clean-all
make
```

This a while as it builds a lot of the LLVM toolchain from scratch.

When it finishes, check things are up to date with: 

```bash
/usr/local/faasm/toolchain/bin/clang --version
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
