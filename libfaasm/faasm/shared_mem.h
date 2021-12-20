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
#define FAASM_TYPE_DOUBLE 3
#define FAASM_TYPE_FLOAT 4

#define FAASM_OP_OVERWRITE 0
#define FAASM_OP_SUM 1
#define FAASM_OP_PRODUCT 2
#define FAASM_OP_SUBTRACT 3
#define FAASM_OP_MAX 4
#define FAASM_OP_MIN 5

#define FAASM_REDUCE(var, type, op) \
    __faasm_sm_reduce((void*)&(var), type, op);

#define FAASM_SHARED_ARRAY(var, type, len) \
    __faasm_sm_array((void*)&(var), type, len);

#define FAASM_SHARED_VAR(var, type) \
    __faasm_sm_var((void*)&(var), type);

#ifdef __cplusplus
}
#endif

#endif
