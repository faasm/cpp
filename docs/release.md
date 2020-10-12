# Building and Releasing

This repo has two types of releases:

- LLVM releases e.g. `vllvm-10.0.1`, which include the bundled LLVM tooling
- Normal releases e.g. `v0.0.1`, which include a bundled sysroot

## Docker

The toolchain uses two Docker images:

- `faasm/toolchain` - used to build the custom LLVM tools, not rebuilt regularly
- `faasm/sysroot` - used to build and package the sysroot

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

