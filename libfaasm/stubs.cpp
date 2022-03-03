#ifdef __wasm__
#error "Should not be building these stubs to wasm"
#endif

#include <cstdio>
#include <cstdlib>

#include <faasm/host_interface.h>

// ----------------------------------
// HOST INTERFACE STUBS
//
// This file is just here to allow users to compile functions natively to please
// clang-format and other editor tools, we do not support actually executing
// libfaasm natively.
// ----------------------------------

#define ERR_MSG "Called unimplemented Faasm host interface stub\n"

long __faasm_read_state(const char* key, unsigned char* buffer, long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned char* __faasm_read_state_ptr(const char* key, long totalLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_write_state(const char* key,
                         const unsigned char* data,
                         long dataLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_append_state(const char* key,
                          const unsigned char* data,
                          long dataLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_read_appended_state(const char* key,
                                 unsigned char* data,
                                 long dataLen,
                                 long nElems)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_clear_appended_state(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_write_state_offset(const char* key,
                                long totalLen,
                                long offset,
                                const unsigned char* data,
                                long dataLen)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned int __faasm_write_state_from_file(const char* key,
                                           const char* filePath)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_read_state_offset(const char* key,
                               long totalLen,
                               long offset,
                               unsigned char* buffer,
                               long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_flag_state_dirty(const char* key, long totalLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_flag_state_offset_dirty(const char* key,
                                     long totalLen,
                                     long offset,
                                     long dataLen)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned char* __faasm_read_state_offset_ptr(const char* key,
                                             long totalLen,
                                             long offset,
                                             long len)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_push_state(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_push_state_partial(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_push_state_partial_mask(const char* key, const char* maskKey)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_pull_state(const char* key, long stateLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_lock_state_global(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_unlock_state_global(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_lock_state_read(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_unlock_state_read(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_lock_state_write(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_unlock_state_write(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

long __faasm_read_input(unsigned char* buffer, long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_write_output(const unsigned char* output, long outputLen)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned int __faasm_chain_name(const char* name,
                                const unsigned char* inputData,
                                long inputDataSize)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned int __faasm_chain_ptr(int (*funcPtr)(),
                               const unsigned char* inputData,
                               long inputDataSize)
{
    printf(ERR_MSG);
    exit(1);
}

int __faasm_await_call(unsigned int messageId)
{
    printf(ERR_MSG);
    exit(1);
}

int __faasm_await_call_output(unsigned int messageId,
                              unsigned char* buffer,
                              long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_get_py_user(unsigned char* buffer, long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_get_py_func(unsigned char* buffer, long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_get_py_entry(unsigned char* buffer, long bufferLen)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned int __faasm_chain_py(const char* name,
                              const unsigned char* inputData,
                              long inputDataSize)
{
    printf(ERR_MSG);
    exit(1);
}

unsigned int __faasm_conf_flag(const char* key)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_backtrace(const int depth)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_sm_array(void* var, int varType, int arrayLen)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_sm_raw(void* var, int varSize)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_sm_reduce(void* var, int varType, int reduceOp, int currentBatch)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_sm_var(void* var, int varType)
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_sm_critical_local()
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_sm_critical_local_end()
{
    printf(ERR_MSG);
    exit(1);
}

void __faasm_migrate_point(FaasmMigrateEntryPoint f, int arg)
{
    printf(ERR_MSG);
    exit(1);
}
