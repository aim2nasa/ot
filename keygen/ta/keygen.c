#include "keygen.h"
#include <stdlib.h>
#include <string.h>

#define ASSERT_PARAM_TYPE(pt) \
do { \
	if ((pt) != param_types) \
		return TEE_ERROR_BAD_PARAMETERS; \
} while (0)

#define VAL2HANDLE(v) (void *)(uintptr_t)(v)

static uint32_t flags = TEE_DATA_FLAG_ACCESS_READ |
			TEE_DATA_FLAG_ACCESS_WRITE |
			TEE_DATA_FLAG_ACCESS_WRITE_META |
			TEE_DATA_FLAG_SHARE_READ |
			TEE_DATA_FLAG_SHARE_WRITE; 

TEE_Result ta_key_gen_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result result = TEE_SUCCESS;
	TEE_ObjectHandle transient_key = (TEE_ObjectHandle)NULL;
	TEE_ObjectHandle persistent_key = (TEE_ObjectHandle)NULL;
	size_t key_size = 256;
	TEE_ObjectInfo keyInfo;
	char *keyFileName = 0;

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

	DMSG("Input params[0], storage id: %d",params[0].value.a);

	keyFileName = malloc(params[1].memref.size+1);
	memcpy(keyFileName,params[1].memref.buffer,params[1].memref.size);
	keyFileName[params[1].memref.size]=0;
	DMSG("Input params[1], key filename: %s",keyFileName);

	if((result=TEE_CreatePersistentObject(params[0].value.a,
					      params[1].memref.buffer,params[1].memref.size,
					      flags,transient_key,NULL,0,&persistent_key))!=TEE_SUCCESS){
		EMSG("Failed to create a persistent key: 0x%x", result);
		goto cleanup2;
	}
	DMSG("%s persistent object(0x%p) created",keyFileName,(void*)persistent_key);

	TEE_CloseObject(persistent_key);
cleanup2:
	TEE_FreeTransientObject(transient_key);
cleanup1:
	free(keyFileName);
	return result;
}

TEE_Result ta_key_open_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result result = TEE_SUCCESS;
	TEE_ObjectHandle key = (TEE_ObjectHandle)NULL;
	char *keyFileName = 0;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT,TEE_PARAM_TYPE_MEMREF_INPUT,
			  TEE_PARAM_TYPE_VALUE_OUTPUT,TEE_PARAM_TYPE_NONE));

	DMSG("Input params[0], storage id: %d",params[0].value.a);

	keyFileName = malloc(params[1].memref.size+1);
	memcpy(keyFileName,params[1].memref.buffer,params[1].memref.size);
	keyFileName[params[1].memref.size]=0;
	DMSG("Input params[1], key filename: %s",keyFileName);

	if((result=TEE_OpenPersistentObject(params[0].value.a,
					    params[1].memref.buffer,params[1].memref.size,
					    flags,&key))!=TEE_SUCCESS){
		EMSG("Failed to open a persistent key: 0x%x", result);
		goto cleanup1;
	}
	params[2].value.a = (uintptr_t)key;
	DMSG("%s persistent object(0x%p) opened",keyFileName,(void*)key);

cleanup1:
	free(keyFileName);
	return result;
}

TEE_Result ta_key_close_cmd(uint32_t param_types, TEE_Param params[4])
{
	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT,TEE_PARAM_TYPE_NONE,
			  TEE_PARAM_TYPE_NONE,TEE_PARAM_TYPE_NONE));

	DMSG("closing object(0x%u)",params[0].value.a);
	TEE_CloseObject((TEE_ObjectHandle)(uintptr_t)params[0].value.a);
	return TEE_SUCCESS;
}

TEE_Result ta_key_unlink_cmd(uint32_t param_types, TEE_Param params[4])
{
	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT,TEE_PARAM_TYPE_NONE,
			  TEE_PARAM_TYPE_NONE,TEE_PARAM_TYPE_NONE));

	DMSG("unlinking object(0x%u)",params[0].value.a);
	TEE_CloseAndDeletePersistentObject1((TEE_ObjectHandle)(uintptr_t)params[0].value.a);
	return TEE_SUCCESS;
}

TEE_Result ta_key_alloc_enum_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res;
	TEE_ObjectEnumHandle oe;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_OUTPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	res = TEE_AllocatePersistentObjectEnumerator(&oe);
	params[0].value.a = (uintptr_t)oe;
	return res;
}

TEE_Result ta_key_free_enum_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_ObjectEnumHandle oe = VAL2HANDLE(params[0].value.a);

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	TEE_FreePersistentObjectEnumerator(oe);
	return TEE_SUCCESS;
}

TEE_Result ta_key_reset_enum_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_ObjectEnumHandle oe = VAL2HANDLE(params[0].value.a);

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	TEE_ResetPersistentObjectEnumerator(oe);
	return TEE_SUCCESS;
}

TEE_Result ta_key_start_enum_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_ObjectEnumHandle oe = VAL2HANDLE(params[0].value.a);

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	return TEE_StartPersistentObjectEnumerator(oe, params[0].value.b);
}

TEE_Result ta_key_next_enum_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_ObjectEnumHandle oe = VAL2HANDLE(params[0].value.a);
	TEE_ObjectInfo *obj;

	if (TEE_PARAM_TYPE_GET(param_types, 0) != TEE_PARAM_TYPE_VALUE_INPUT)
		return TEE_ERROR_BAD_PARAMETERS;
	if (TEE_PARAM_TYPE_GET(param_types, 2) != TEE_PARAM_TYPE_MEMREF_OUTPUT)
		return TEE_ERROR_BAD_PARAMETERS;
	if (TEE_PARAM_TYPE_GET(param_types, 3) != TEE_PARAM_TYPE_NONE)
		return TEE_ERROR_BAD_PARAMETERS;

	if (TEE_PARAM_TYPE_GET(param_types, 1) == TEE_PARAM_TYPE_NONE)
		obj = NULL;
	else if (TEE_PARAM_TYPE_GET(param_types, 1) ==
		 TEE_PARAM_TYPE_MEMREF_OUTPUT) {
		if (params[1].memref.size < sizeof(TEE_ObjectInfo)) {
			params[1].memref.size = sizeof(TEE_ObjectInfo);
			return TEE_ERROR_SHORT_BUFFER;
		}
		params[1].memref.size = sizeof(TEE_ObjectInfo);
		obj = (TEE_ObjectInfo *)params[1].memref.buffer;
	} else
		return TEE_ERROR_BAD_PARAMETERS;

	if (params[2].memref.size < TEE_OBJECT_ID_MAX_LEN)
		return TEE_ERROR_SHORT_BUFFER;

	return TEE_GetNextPersistentObject(oe, obj,
					   params[2].memref.buffer,
					   &params[2].memref.size);
}

TEE_Result ta_key_alloc_oper_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_Result res;
	TEE_OperationHandle op;

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_OUTPUT, TEE_PARAM_TYPE_VALUE_INPUT,
			   TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_VALUE_INPUT));

	res = TEE_AllocateOperation(&op,params[1].value.a,params[2].value.a,params[3].value.a);
	params[0].value.a = (uintptr_t)op;
	return res;
}

TEE_Result ta_key_free_oper_cmd(uint32_t param_types, TEE_Param params[4])
{
	TEE_OperationHandle op = VAL2HANDLE(params[1].value.a);

	ASSERT_PARAM_TYPE(TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE));

	TEE_FreeOperation(op);
	return TEE_SUCCESS;
}
