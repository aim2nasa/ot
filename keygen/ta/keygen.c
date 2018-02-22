#include "keygen.h"

#define ASSERT_PARAM_TYPE(pt) \
do { \
	if ((pt) != param_types) \
		return TEE_ERROR_BAD_PARAMETERS; \
} while (0)

TEE_Result ta_keygen_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result result = TEE_SUCCESS;
	TEE_ObjectHandle transient_key = (TEE_ObjectHandle)NULL;
	size_t key_size = 256;
	TEE_ObjectInfo keyInfo;

	(void)params;
	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT,TEE_PARAM_TYPE_MEMREF_INPUT,
			  TEE_PARAM_TYPE_NONE,TEE_PARAM_TYPE_NONE));

	if((result=TEE_AllocateTransientObject(TEE_TYPE_AES,key_size,&transient_key))!=TEE_SUCCESS){
		EMSG("Failed to Allocate transient object handle : 0x%x",result);
		goto cleanup1;
	}
	DMSG("Allocated TransientObject: 0x%p",(void*)transient_key);


	if((result=TEE_GenerateKey(transient_key,key_size,NULL,0))!=TEE_SUCCESS){
		EMSG("Failed to generate a transient key: 0x%x", result);
		goto cleanup2;
	}
	DMSG("Key(%zd bit) generated with TransientObject(0x%p)",key_size,(void*)transient_key);

	TEE_GetObjectInfo1(transient_key, &keyInfo);
	DMSG("keyInfo: %zd bytes",sizeof(TEE_ObjectInfo));
cleanup2:
	TEE_FreeTransientObject(transient_key);
cleanup1:
	return TEE_SUCCESS;
}
