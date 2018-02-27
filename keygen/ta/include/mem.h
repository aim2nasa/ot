#ifndef MEM_H
#define MEM_H

#include <tee_api.h>

TEE_Result ta_mem_alloc_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_mem_free_cmd(uint32_t param_types, TEE_Param params[4]);

#endif
