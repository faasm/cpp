# Faasm OpenMP Library

This library provides the compile-time support for OpenMP needed by Faasm
functions.

`omp.h` is copied from Clang 8.0.1. (actual OMP version is detailed the file
itself).

## Adding support for new OpenMP runtime functions

The definition of Faasm's OpenMP interface can be found in the [`libfaasmp`
directory](/libfaasmp).

To add a new function you need to include it in both the `libfaasmp` definition
and the Faasm host interface.

## Assembly file

LLVM adds GNU compatibility data symbols on some features for synchronisation.
The wasm linker needs to be aware of the sizes and the names of the symbols,
which are not valid C/C++ identifiers, and as such are resolved in assembly in
`libfaasmp/z_Linux_asm.S` file.

An error from the linker when compiling new parts of the API implies you need
to add the symbol to this file.

