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

#define FAASM_TYPE_BOOL 0
#define FAASM_TYPE_INT 1
#define FAASM_TYPE_DOUBLE 2
#define FAASM_TYPE_FLOAT 3

#define FAASM_OP_SUM 0
#define FAASM_OP_SUB 1
#define FAASM_OP_MULT 2
#define FAASM_OP_DIV 3
#define FAASM_OP_MIN 4
#define FAASM_OP_MAX 5

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
