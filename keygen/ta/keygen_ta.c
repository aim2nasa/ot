#include <tee_internal_api.h>
#include <keygen_ta.h>
#include <keygen.h>

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
	case TA_KEY_GEN_CMD:
		IMSG("KeyGen CMD\n");
		return ta_key_gen_cmd(param_types,params);
	case TA_KEY_OPEN_CMD:
		IMSG("KeyOpen CMD\n");
		return ta_key_open_cmd(param_types,params);
	case TA_KEY_CLOSE_CMD:
		IMSG("KeyClose CMD\n");
		return ta_key_close_cmd(param_types,params);
	case TA_KEY_UNLINK_CMD:
		IMSG("KeyUnlink CMD\n");
		return ta_key_unlink_cmd(param_types,params);
	case TA_KEY_ALLOC_ENUM_CMD:
		IMSG("KeyAllocEnum CMD\n");
		return ta_key_alloc_enum_cmd(param_types,params);
	case TA_KEY_FREE_ENUM_CMD:
		IMSG("KeyFreeEnum CMD\n");
		return ta_key_free_enum_cmd(param_types,params);
	case TA_KEY_RESET_ENUM_CMD:
		IMSG("KeyResetEnum CMD\n");
		return ta_key_reset_enum_cmd(param_types,params);
	case TA_KEY_START_ENUM_CMD:
		IMSG("KeyStartEnum CMD\n");
		return ta_key_start_enum_cmd(param_types,params);
	case TA_KEY_NEXT_ENUM_CMD:
		IMSG("KeyNextEnum CMD\n");
		return ta_key_next_enum_cmd(param_types,params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}