#include "mem.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

TEE_Result ta_mem_alloc_cmd(uint32_t param_types, TEE_Param params[4])
{
	void *mem;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_OUTPUT, TEE_PARAM_TYPE_VALUE_INPUT,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	mem = TEE_Malloc(params[1].value.a,0);
	params[0].value.a = (uintptr_t)mem;
	return TEE_SUCCESS;
}

TEE_Result ta_mem_free_cmd(uint32_t param_types, TEE_Param params[4])
{
	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	TEE_Free(VAL2HANDLE(params[0].value.a));
	return TEE_SUCCESS;
}
