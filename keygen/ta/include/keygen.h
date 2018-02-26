#ifndef KEYGEN_H
#define KEYGEN_H

#include <tee_api.h>

TEE_Result ta_key_gen_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_open_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_close_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_key_unlink_cmd(uint32_t param_types, TEE_Param params[4]);

#endif
