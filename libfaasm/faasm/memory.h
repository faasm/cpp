#pragma once

#ifdef __wasm__
// WAMR performs some optimisations on the WASM modules at load time, whereby
// if no memory.grow op-code is found, memory is shrunk to the minimum
// (as specified by the default heap and stack size). This optimisation breaks
// Faasm's memory management, as we may sometimes want to enlarge the WASM's
// memory from outside WASM code. To prevent this optimisation, every time we
// build a WASM function for its usage in faasm (and include the faasm/faasm.h
// header) we link with this dummy function that uses the memory.grow op-code.
// Note that this function is actually not called. See:
// https://github.com/bytecodealliance/wasm-micro-runtime/issues/1706
void __attribute__((used)) __faasm_memory_layout_protection()
{
    __builtin_wasm_memory_grow(0, 0);
}
#endif
