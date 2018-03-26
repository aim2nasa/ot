#include <tee_internal_api.h>
#include <keygen_ta.h>
#include <keygen.h>
#include <mem.h>
#include <cipher.h>

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
	case TA_KEY_ALLOC_OPER_CMD:
		IMSG("KeyAllocOper CMD\n");
		return ta_key_alloc_oper_cmd(param_types,params);
	case TA_KEY_FREE_OPER_CMD:
		IMSG("KeyFreeOper CMD\n");
		return ta_key_free_oper_cmd(param_types,params);
	case TA_KEY_SETKEY_OPER_CMD:
		IMSG("KeySetkeyOper CMD\n");
		return ta_key_setkey_oper_cmd(param_types,params);
	case TA_KEY_GET_OBJECT_BUFFER_ATTRIBUTE_CMD:
		IMSG("KeyGetObjectBufferAttribute CMD\n");
		return ta_key_get_object_buffer_attribute_cmd(param_types,params);
	case TA_KEY_GET_OBJECT_VALUE_ATTRIBUTE_CMD:
		IMSG("KeyGetObjectValueAttribute CMD\n");
		return ta_key_get_object_value_attribute_cmd(param_types,params);
	case TA_KEY_INJECT_CMD:
		IMSG("KeyInject CMD\n");
		return ta_key_inject_cmd(param_types,params);
	case TA_MEM_ALLOC_CMD:
		IMSG("MemAlloc CMD\n");
		return ta_mem_alloc_cmd(param_types,params);
	case TA_MEM_FREE_CMD:
		IMSG("MemFree CMD\n");
		return ta_mem_free_cmd(param_types,params);
	case TA_CIPHER_INIT_CMD:
		IMSG("CipherInit CMD\n");
		return ta_cipher_init_cmd(param_types,params);
	case TA_CIPHER_UPDATE_CMD:
		IMSG("CipherUpdate CMD\n");
		return ta_cipher_update_cmd(param_types,params);
	case TA_CIPHER_DO_FINAL_CMD:
		IMSG("CipherDoFinal CMD\n");
		return ta_cipher_do_final_cmd(param_types,params);
	default:
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
