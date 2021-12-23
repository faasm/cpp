#ifndef FAASMC_SHARED_MEM_H
#define FAASMC_SHARED_MEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <faasm/host_interface.h>

/**
 * Faasm shared memory markup
 */

// NOTE - these types must correspond to the enums defined in the Faabric
// codebase, stored here at time of writing:
// https://github.com/faasm/faabric/blob/master/include/faabric/util/snapshot.h
#define FAASM_TYPE_RAW 0
#define FAASM_TYPE_BOOL 1
#define FAASM_TYPE_INT 2
#define FAASM_TYPE_LONG 3
#define FAASM_TYPE_FLOAT 4
#define FAASM_TYPE_DOUBLE 5

#define FAASM_OP_OVERWRITE 0
#define FAASM_OP_SUM 1
#define FAASM_OP_PRODUCT 2
#define FAASM_OP_SUBTRACT 3
#define FAASM_OP_MAX 4
#define FAASM_OP_MIN 5

#define FAASM_REDUCE(var, type, op) __faasm_sm_reduce((void*)&(var), type, op);

#define FAASM_SHARED_ARRAY(var, type, len)                                     \
    __faasm_sm_array((void*)&(var), type, len);

#define FAASM_SHARED_RAW(var, len) __faasm_sm_raw((void*)&(var), len);

#define FAASM_SHARED_VAR(var, type) __faasm_sm_var((void*)&(var), type);

#define FAASM_DEFAULT_SHARED() __faasm_sm_default_shared();

#define FAASM_START_CRITICAL_LOCAL() __faasm_sm_critical_local();

#define FAASM_END_CRITICAL_LOCAL() __faasm_sm_critical_local_end();

#define FAASM_CRITICAL_LOCAL(op)                                               \
    __faasm_sm_critical_local();                                               \
    {                                                                          \
        op;                                                                    \
    }                                                                          \
    __faasm_sm_critical_local_end();

#define FAASM_ATOMIC_INCR(var)                                                 \
    FAASM_REDUCE(var, FAASM_TYPE_INT, FAASM_OP_SUM)                            \
    FAASM_CRITICAL_LOCAL(var++)

#define FAASM_ATOMIC_INCR_BY(var, value)                                       \
    FAASM_REDUCE(var, FAASM_TYPE_INT, FAASM_OP_SUM)                            \
    FAASM_CRITICAL_LOCAL(var += value)

#ifdef __cplusplus
}
#endif

#endif
