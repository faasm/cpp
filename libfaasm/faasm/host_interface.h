#ifndef FAASM_HOST_INTERFACE_H
#define FAASM_HOST_INTERFACE_H

#ifdef __wasm__
#define HOST_IFACE_FUNC
#else
// Avoid need to define any of these functions when compiling natively
#define HOST_IFACE_FUNC __attribute__((weak))
#endif

// Migration entry point
typedef void (*FaasmMigrateEntryPoint)(int);

HOST_IFACE_FUNC
long __faasm_read_state(const char* key, unsigned char* buffer, long bufferLen);

HOST_IFACE_FUNC
unsigned char* __faasm_read_state_ptr(const char* key, long totalLen);

HOST_IFACE_FUNC
void __faasm_write_state(const char* key,
                         const unsigned char* data,
                         long dataLen);

HOST_IFACE_FUNC
void __faasm_append_state(const char* key,
                          const unsigned char* data,
                          long dataLen);

HOST_IFACE_FUNC
void __faasm_read_appended_state(const char* key,
                                 unsigned char* data,
                                 long dataLen,
                                 long nElems);

HOST_IFACE_FUNC
void __faasm_clear_appended_state(const char* key);

HOST_IFACE_FUNC
void __faasm_write_state_offset(const char* key,
                                long totalLen,
                                long offset,
                                const unsigned char* data,
                                long dataLen);

HOST_IFACE_FUNC
unsigned int __faasm_write_state_from_file(const char* key,
                                           const char* filePath);

HOST_IFACE_FUNC
void __faasm_read_state_offset(const char* key,
                               long totalLen,
                               long offset,
                               unsigned char* buffer,
                               long bufferLen);

HOST_IFACE_FUNC
void __faasm_flag_state_dirty(const char* key, long totalLen);

HOST_IFACE_FUNC
void __faasm_flag_state_offset_dirty(const char* key,
                                     long totalLen,
                                     long offset,
                                     long dataLen);

HOST_IFACE_FUNC
unsigned char* __faasm_read_state_offset_ptr(const char* key,
                                             long totalLen,
                                             long offset,
                                             long len);

HOST_IFACE_FUNC
void __faasm_push_state(const char* key);

HOST_IFACE_FUNC
void __faasm_push_state_partial(const char* key);

HOST_IFACE_FUNC
void __faasm_push_state_partial_mask(const char* key, const char* maskKey);

HOST_IFACE_FUNC
void __faasm_pull_state(const char* key, long stateLen);

HOST_IFACE_FUNC
void __faasm_lock_state_read(const char* key);

HOST_IFACE_FUNC
void __faasm_unlock_state_read(const char* key);

HOST_IFACE_FUNC
void __faasm_lock_state_write(const char* key);

HOST_IFACE_FUNC
void __faasm_unlock_state_write(const char* key);

HOST_IFACE_FUNC
long __faasm_read_input(unsigned char* buffer, long bufferLen);

HOST_IFACE_FUNC
void __faasm_write_output(const char* output, long outputLen);

HOST_IFACE_FUNC
unsigned int __faasm_chain_name(const char* name,
                                const unsigned char* inputData,
                                long inputDataSize);

HOST_IFACE_FUNC
unsigned int __faasm_chain_ptr(int (*funcPtr)(),
                               const unsigned char* inputData,
                               long inputDataSize);

HOST_IFACE_FUNC
int __faasm_await_call(unsigned int messageId);

HOST_IFACE_FUNC
int __faasm_await_call_output(unsigned int messageId,
                              const char* output,
                              long outputLen);

HOST_IFACE_FUNC
void __faasm_get_py_user(unsigned char* buffer, long bufferLen);

HOST_IFACE_FUNC
void __faasm_get_py_func(unsigned char* buffer, long bufferLen);

HOST_IFACE_FUNC
void __faasm_get_py_entry(unsigned char* buffer, long bufferLen);

HOST_IFACE_FUNC
unsigned int __faasm_chain_py(const char* name,
                              const unsigned char* inputData,
                              long inputDataSize);

HOST_IFACE_FUNC
unsigned int __faasm_conf_flag(const char* key);

HOST_IFACE_FUNC
void __faasm_backtrace(const int depth);

HOST_IFACE_FUNC
void __faasm_sm_array(void* var, int varType, int arrayLen);

HOST_IFACE_FUNC
void __faasm_sm_raw(void* var, int varSize);

HOST_IFACE_FUNC
void __faasm_sm_reduce(void* var, int varType, int reduceOp, int currentBatch);

HOST_IFACE_FUNC
void __faasm_sm_var(void* var, int varType);

HOST_IFACE_FUNC
void __faasm_sm_critical_local();

HOST_IFACE_FUNC
void __faasm_sm_critical_local_end();

HOST_IFACE_FUNC
void __faasm_migrate_point(FaasmMigrateEntryPoint f, int arg);

HOST_IFACE_FUNC
void __faasm_host_interface_test(int testNum);

// ----- S3 -----

HOST_IFACE_FUNC
int __faasm_s3_get_num_buckets();

// Note that bucketsBuffer is, in reality, a char** populated by the host
HOST_IFACE_FUNC
void __faasm_s3_list_buckets(void* bucketsBuffer, int* bucketsBufferLens);

HOST_IFACE_FUNC
int __faasm_s3_get_num_keys(const char* bucketName);

// Note that keysBuffer is, in reality, a char** populated by the host
HOST_IFACE_FUNC
void __faasm_s3_list_keys(const char* bucketName,
                          void* keysBuffer,
                          int* keysBufferLens);

HOST_IFACE_FUNC
int __faasm_s3_add_key_bytes(const char* bucketName,
                             const char* keyName,
                             void* keyBuffer,
                             int keyBufferLen);

// Note that keyBuffer is, in reality, a uint8_t** populated by the host
HOST_IFACE_FUNC
int __faasm_s3_get_key_bytes(const char* bucketName,
                             const char* keyName,
                             void* keyBuffer,
                             int* keyBufferLen);
#endif
