#ifndef CIPHER_H
#define CIPHER_H

#include <tee_api.h>

TEE_Result ta_cipher_init_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_cipher_update_cmd(uint32_t param_types, TEE_Param params[4]);
TEE_Result ta_cipher_do_final_cmd(uint32_t param_types, TEE_Param params[4]);

#endif
