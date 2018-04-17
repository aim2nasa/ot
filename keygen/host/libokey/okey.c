#include "okey.h"
#include <stdlib.h>
#include <string.h>
#include <keygen_ta.h>
#include <assert.h>
#include "enumHelper.h"
#include <common.h>

#define TEEC_OPERATION_INITIALIZER      { 0 }
TEEC_UUID uuid = TA_KEYGEN_UUID;

TEEC_Result allocShm(okey *o,TEEC_SharedMemory *shm,size_t size)
{
	shm->buffer = NULL;
	shm->size = size;
	return TEEC_AllocateSharedMemory(o->ctx,shm);
}

void freeShm(TEEC_SharedMemory *shm)
{
	TEEC_ReleaseSharedMemory(shm);
}

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

TEEC_Result openSession(okey *o,uint32_t connectionMethod,
			const void *connectionData,TEEC_Operation *operation)
{
	o->session = (TEEC_Session*)malloc(sizeof(TEEC_Session));
	return TEEC_OpenSession(o->ctx,o->session,&uuid,connectionMethod,
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

	op.params[0].value.a = storageId;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE,TEEC_NONE);

	return TEEC_InvokeCommand(o->session,TA_KEY_GEN_CMD,&op,&o->error);
}

TEEC_Result keyOpen(okey *o,uint32_t storageId,const char *keyFileName,uint32_t *keyObj)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = storageId;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_OUTPUT,TEEC_NONE);

	res = TEEC_InvokeCommand(o->session,TA_KEY_OPEN_CMD,&op,&o->error);
	if(res==TEEC_SUCCESS) *keyObj = op.params[2].value.a;
	return res;
}

TEEC_Result keyClose(okey *o,uint32_t keyObj)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);

	return TEEC_InvokeCommand(o->session,TA_KEY_CLOSE_CMD,&op,&o->error);
}

TEEC_Result keyUnlink(okey *o,uint32_t keyObj)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);

	return TEEC_InvokeCommand(o->session,TA_KEY_UNLINK_CMD,&op,&o->error);
}

TEEC_Result keyInject(okey *o,uint32_t storageId,const char *keyFileName,uint8_t *keyBuffer,size_t keySize)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = storageId;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.params[2].tmpref.buffer = keyBuffer ;
	op.params[2].tmpref.size = keySize;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE);

	return TEEC_InvokeCommand(o->session,TA_KEY_INJECT_CMD,&op,&o->error);
}

TEEC_Result keyGetObjectBufferAttribute(okey *o,uint32_t keyObj,uint32_t attrId,void *buffer,size_t *bufferSize)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

        assert((uintptr_t)keyObj <= UINT32_MAX);
        op.params[0].value.a = keyObj;
        op.params[0].value.b = attrId;

        op.params[1].tmpref.buffer = buffer;
        op.params[1].tmpref.size = *bufferSize;

        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
                                         TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE,
                                         TEEC_NONE);

        res = TEEC_InvokeCommand(o->session,TA_KEY_GET_OBJECT_BUFFER_ATTRIBUTE_CMD,&op,&o->error);
        if (res == TEEC_SUCCESS) *bufferSize = op.params[1].tmpref.size;
        return res;
}

TEEC_Result keyEnumObjectList(okey *o,uint32_t storageId,eObjList **list)
{
	TEEC_Result res;
	uint32_t enumHandle;
	size_t infoSize;
	uint32_t idSize;
	uint8_t info[256];
	uint8_t id[256+1];	//+1 to include null at the end of name
	eObjList *prev = NULL;

	*list = NULL;

	res = fs_alloc_enum(o->session,&enumHandle);
        if(res!=TEEC_SUCCESS) return res;

	res = fs_start_enum(o->session,enumHandle,storageId);
        if(res!=TEEC_SUCCESS) return res;

	memset((void*)info,0,sizeof(info));
	memset((void*)id,0,sizeof(id));
	infoSize = sizeof(info);
	idSize = sizeof(id);

	while(TEEC_SUCCESS==fs_next_enum(o->session,enumHandle,info,&infoSize,id,&idSize)){
		eObj *obj = NULL;
		eObjList *objectList = (eObjList*)malloc(sizeof(eObjList));

		if(*list==NULL) *list = objectList; 

		obj = (eObj*)malloc(sizeof(eObj));
		obj->idSize = idSize;
		obj->id = (uint8_t*)malloc(idSize);
		memcpy(obj->id,id,idSize);

		objectList->object = obj;
		objectList->next = NULL;
		if(prev) prev->next = objectList;

		prev = objectList;
		infoSize = sizeof(info);
		idSize = sizeof(id);
	}

	res = fs_free_enum(o->session,enumHandle);
        if(res!=TEEC_SUCCESS) return res;

	return res;
}

int keyFreeEnumObjectList(eObjList *list)
{
	int nCount = 0;
	eObjList *cur = list;
	while(cur){
		eObjList *next = cur->next;
		free(cur->object->id);
		free(cur->object);
		free(cur);
		if(next) {
			cur = next;
		}else{
			cur = NULL;
		}
		nCount++;
	}
	return nCount;
}

TEEC_Result keyAllocOper(okey *o,bool bEnc,size_t keySize,TEE_OperationHandle *encOp)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[1].value.a = TEE_ALG_AES_ECB_NOPAD;	//does not require IV
	op.params[2].value.a = bEnc?TEE_MODE_ENCRYPT:TEE_MODE_DECRYPT;
	op.params[3].value.a = keySize;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT);

	res = TEEC_InvokeCommand(o->session,TA_KEY_ALLOC_OPER_CMD,&op,&o->error);
	if(res==TEEC_SUCCESS) *encOp = VAL2HANDLE(op.params[0].value.a);
	return res;
}

TEEC_Result keyFreeOper(okey *o,TEE_OperationHandle encOp)
{
        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = (uintptr_t)encOp;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	return TEEC_InvokeCommand(o->session,TA_KEY_FREE_OPER_CMD,&op,&o->error);
}       

TEEC_Result keySetkeyOper(okey *o,TEE_OperationHandle encOp,uint32_t keyObj)
{
        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

        op.params[0].value.a = (uintptr_t)encOp;
        op.params[1].value.a = keyObj;
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE);
        return TEEC_InvokeCommand(o->session,TA_KEY_SETKEY_OPER_CMD,&op,&o->error);
}
