#include <tee_internal_api.h>
#include <persistentObj_ta.h>

#define ASSERT_PARAM_TYPE(pt) \
do { \
        if ((pt) != param_types) \
                return TEE_ERROR_BAD_PARAMETERS; \
} while (0)

#define VAL2HANDLE(v) (void *)(uintptr_t)(v)

TEE_Result ta_storage_cmd_create(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_storage_cmd_unlink(uint32_t param_types, TEE_Param params[4]);

TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
	TEE_Param __maybe_unused params[4],
	void __maybe_unused **sess_ctx)
{
	DMSG("has been called");

	(void)&param_types;

	return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	DMSG("has been called");
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
	uint32_t cmd_id,
	uint32_t param_types,
	TEE_Param params[4])
{
	DMSG("has been called");

	(void)&cmd_id;
	(void)&param_types;
	(void)&params;

	switch(cmd_id) {
	case TA_PERSISTENTOBJ_CMD_CREATE:
		IMSG("PersistentObj Create command received");
                return ta_storage_cmd_create(param_types,params);

	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}

TEE_Result ta_storage_cmd_create(uint32_t param_types, TEE_Param params[4])
{
        TEE_Result res;
        TEE_ObjectHandle o;

        DMSG("has been called");
        ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
                          (TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_VALUE_INOUT,
                           TEE_PARAM_TYPE_VALUE_INPUT,
                           TEE_PARAM_TYPE_MEMREF_INPUT));

        res = TEE_CreatePersistentObject(params[2].value.b,
                 params[0].memref.buffer, params[0].memref.size,
                 params[1].value.a,
                 (TEE_ObjectHandle)(uintptr_t)params[2].value.a,
                 params[3].memref.buffer, params[3].memref.size, &o);
        params[1].value.b = (uintptr_t)o;
        return res;
}

TEE_Result ta_storage_cmd_unlink(uint32_t param_types, TEE_Param params[4])
{
	TEE_ObjectHandle o = VAL2HANDLE(params[0].value.a);

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	TEE_CloseAndDeletePersistentObject1(o);

	return TEE_SUCCESS;
}
