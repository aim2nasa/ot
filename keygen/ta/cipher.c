#include "cipher.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

struct __TEE_OperationHandle {
	TEE_OperationInfo info;
	TEE_ObjectHandle key1;
	TEE_ObjectHandle key2;
	uint32_t operationState;/* Operation state : INITIAL or ACTIVE */
	uint8_t *buffer;	/* buffer to collect complete blocks */
	bool buffer_two_blocks;	/* True if two blocks need to be buffered */
	size_t block_size;	/* Block size of cipher */
	size_t buffer_offs;	/* Offset in buffer */
	uint32_t state;		/* Handle to state in TEE Core */
	uint32_t ae_tag_len;	/*
				 * tag_len in bytes for AE operation else unused
				 */
};

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
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_MEMREF_INPUT,
			   TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE));

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
	TEE_OperationHandle op;
	TEE_Result res;
	size_t mod;
	(void)param_types;
	(void)op;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_MEMREF_INPUT,
			   TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE));

	DMSG("Cipher DoFinal input:%s input buffer size:%d output buffer size:%d",
		(char*)params[1].memref.buffer,params[1].memref.size,params[2].memref.size);

	op = VAL2HANDLE(params[0].value.a);
	DMSG("operation->info.algorithm=0x%x",op->info.algorithm);
	DMSG("operation->buffer_offs=%zd",op->buffer_offs);
	DMSG("srcLen=%u",params[1].memref.size);
	DMSG("operation->block_size=%zd",op->block_size);
	
	mod = (op->buffer_offs+params[1].memref.size)%op->block_size; 
	DMSG("(operation->buffer_offs+srcLen)%%operation->block_size=%zd",mod);
	if(mod!=0) DMSG("mod(%zd)!=0,error TEE_ERROR_BAD_PARAMETERS(0xffff0006) expected",mod); 
	
	res = TEE_CipherDoFinal(VAL2HANDLE(params[0].value.a),
			       params[1].memref.buffer,params[1].memref.size,
			       params[2].memref.buffer,&params[2].memref.size);
	DMSG("Updated output size:%d",params[2].memref.size);
	return res;
}
