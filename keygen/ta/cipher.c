#include "cipher.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

TEE_Result ta_cipher_init_cmd(uint32_t param_types, TEE_Param params[4])
{
	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_MEMREF_INPUT,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));
	
	TEE_CipherInit(VAL2HANDLE(params[0].value.a),params[1].memref.buffer,params[1].memref.size);
	return TEE_SUCCESS;
}

TEE_Result ta_cipher_update_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_MEMREF_INOUT,
			   TEE_PARAM_TYPE_MEMREF_INOUT, TEE_PARAM_TYPE_NONE));

	DMSG("Cipher Update input:%s input buffer size:%d output buffer size:%d",
		(char*)params[1].memref.buffer,params[1].memref.size,params[2].memref.size);

	res = TEE_CipherUpdate(VAL2HANDLE(params[0].value.a),
			       params[1].memref.buffer,params[1].memref.size,
			       params[2].memref.buffer,&params[2].memref.size);
	DMSG("Updated output size:%d",params[2].memref.size);
	return res;
}

TEE_Result ta_cipher_do_final_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_MEMREF_INOUT,
			   TEE_PARAM_TYPE_MEMREF_INOUT, TEE_PARAM_TYPE_NONE));

	DMSG("Cipher DoFinal input:%s input buffer size:%d output buffer size:%d",
		(char*)params[1].memref.buffer,params[1].memref.size,params[2].memref.size);

	res = TEE_CipherDoFinal(VAL2HANDLE(params[0].value.a),
			       params[1].memref.buffer,params[1].memref.size,
			       params[2].memref.buffer,&params[2].memref.size);
	DMSG("Updated output size:%d",params[2].memref.size);
	return res;
}
