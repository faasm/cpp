# Upgrading LLVM

To upgrade the underlying LLVM version you need to:

- Update the _submodule_ in this project (see below)
- Update the version in `docker/sysroot.dockerfile`
- Update the `versions.yml` file
- Update `Makefile`
- Run the `inv container.toolchain` task
- Test it out
- Run the `inv container.push-toolchain` task

## Updating the LLVM submodule

Find the commit related to tag name for the desired release in
[the LLVM repo](https://github.com/llvm/llvm-project/releases) (e.g.
`llvmorg-10.0.1`), then:

```bash
cd third-party/llvm-project
git checkout master
git fetch origin
git checkout <tag-name>
```

# Rebuilding LLVM

To rebuild LLVM, there a couple of options. To rebuild just libc:

```bash
make clean-libc
make
```

To rebuilding all the libs, i.e. libc, libcxx, libcxxabi and
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
