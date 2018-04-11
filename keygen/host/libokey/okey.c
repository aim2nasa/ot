#include <stdlib.h>
#include "okey.h"
#include <string.h>
#include <keygen_ta.h>

#define TEEC_OPERATION_INITIALIZER      { 0 }

TEEC_Result initializeContext(const char *name,okey *o)
{
	o->ctx = (TEEC_Context*)malloc(sizeof(TEEC_Context));
	return TEEC_InitializeContext(name,o->ctx);
}

void finalizeContext(okey *o)
{
	TEEC_FinalizeContext(o->ctx);
	free(o->ctx);
}

TEEC_Result openSession(okey *o,const TEEC_UUID *destination,uint32_t connectionMethod,
			const void *connectionData,TEEC_Operation *operation)
{
	o->session = (TEEC_Session*)malloc(sizeof(TEEC_Session));
	return TEEC_OpenSession(o->ctx,o->session,destination,connectionMethod,
		connectionData,operation,&o->error);
}

void closeSession(okey *o)
{
	TEEC_CloseSession(o->session);
	free(o->session);
}

TEEC_Result keyGen(okey *o,uint32_t storageId,const char *keyFileName)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = TEE_STORAGE_PRIVATE;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE,TEEC_NONE);

	return TEEC_InvokeCommand(o->session,TA_KEY_GEN_CMD,&op,&o->error);
}
