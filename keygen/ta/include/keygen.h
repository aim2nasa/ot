#ifndef KEYGEN_H
#define KEYGEN_H

#include <tee_api.h>

TEE_Result ta_key_gen_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_open_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_close_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_unlink_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_alloc_enum_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_free_enum_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_reset_enum_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_start_enum_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_next_enum_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_alloc_oper_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_free_oper_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_setkey_oper_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_get_object_buffer_attribute_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_get_object_value_attribute_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_inject_cmd(uint32_t param_types, TEE_Param params[4]);

#endif
