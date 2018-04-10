#include <stdlib.h>
#include "okey.h"

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
		connectionData,operation,&o->returnOrigin);
}

void closeSession(okey *o)
{
	TEEC_CloseSession(o->session);
	free(o->session);
}
