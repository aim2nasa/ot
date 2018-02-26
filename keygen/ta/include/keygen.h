#ifndef KEYGEN_H
#define KEYGEN_H

#include <tee_api.h>

TEE_Result ta_keygen_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_keyopen_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_keyclose_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_keyunlink_cmd(uint32_t param_types, TEE_Param params[4]);

#endif
