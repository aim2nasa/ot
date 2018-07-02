#include "okey.h"
#include <stdlib.h>
#include <string.h>
#include <keygen_ta.h>
#include <assert.h>
#include "enumHelper.h"
#include <common.h>

#define TEEC_OPERATION_INITIALIZER      { 0 }

/* API Error Codes */
#define TEE_SUCCESS                       0x00000000
#define TEE_ERROR_CORRUPT_OBJECT          0xF0100001
#define TEE_ERROR_CORRUPT_OBJECT_2        0xF0100002
#define TEE_ERROR_STORAGE_NOT_AVAILABLE   0xF0100003
#define TEE_ERROR_STORAGE_NOT_AVAILABLE_2 0xF0100004
#define TEE_ERROR_GENERIC                 0xFFFF0000
#define TEE_ERROR_ACCESS_DENIED           0xFFFF0001
#define TEE_ERROR_CANCEL                  0xFFFF0002
#define TEE_ERROR_ACCESS_CONFLICT         0xFFFF0003
#define TEE_ERROR_EXCESS_DATA             0xFFFF0004
#define TEE_ERROR_BAD_FORMAT              0xFFFF0005
#define TEE_ERROR_BAD_PARAMETERS          0xFFFF0006
#define TEE_ERROR_BAD_STATE               0xFFFF0007
#define TEE_ERROR_ITEM_NOT_FOUND          0xFFFF0008
#define TEE_ERROR_NOT_IMPLEMENTED         0xFFFF0009
#define TEE_ERROR_NOT_SUPPORTED           0xFFFF000A
#define TEE_ERROR_NO_DATA                 0xFFFF000B
#define TEE_ERROR_OUT_OF_MEMORY           0xFFFF000C
#define TEE_ERROR_BUSY                    0xFFFF000D
#define TEE_ERROR_COMMUNICATION           0xFFFF000E
#define TEE_ERROR_SECURITY                0xFFFF000F
#define TEE_ERROR_SHORT_BUFFER            0xFFFF0010
#define TEE_ERROR_EXTERNAL_CANCEL         0xFFFF0011
#define TEE_ERROR_OVERFLOW                0xFFFF300F
#define TEE_ERROR_TARGET_DEAD             0xFFFF3024
#define TEE_ERROR_STORAGE_NO_SPACE        0xFFFF3041
#define TEE_ERROR_MAC_INVALID             0xFFFF3071
#define TEE_ERROR_SIGNATURE_INVALID       0xFFFF3072
#define TEE_ERROR_TIME_NOT_SET            0xFFFF5000
#define TEE_ERROR_TIME_NEEDS_RESET        0xFFFF5001

/* Algorithm Identifiers */
#define TEE_ALG_AES_ECB_NOPAD                   0x10000010
#define TEE_ALG_AES_CBC_NOPAD                   0x10000110
#define TEE_ALG_AES_CTR                         0x10000210
#define TEE_ALG_AES_CTS                         0x10000310
#define TEE_ALG_AES_XTS                         0x10000410
#define TEE_ALG_AES_CBC_MAC_NOPAD               0x30000110
#define TEE_ALG_AES_CBC_MAC_PKCS5               0x30000510
#define TEE_ALG_AES_CMAC                        0x30000610
#define TEE_ALG_AES_CCM                         0x40000710
#define TEE_ALG_AES_GCM                         0x40000810

TEEC_UUID uuid = TA_KEYGEN_UUID;

typedef enum {
        MODE_ENCRYPT = 0,
        MODE_DECRYPT = 1,
} OperationMode;

static TEEC_SharedMemory in_shm={
        .flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT
};

static TEEC_SharedMemory out_shm={
        .flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT
};

static void copy_shm(TEEC_SharedMemory *shm,void *src,size_t n)
{
        memcpy(shm->buffer,src,n);
}

static void set_shm(TEEC_SharedMemory *shm,size_t n)
{
        memset(shm->buffer,0,n);
}

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
	o->shMemSize = TEE_AES_BLOCK_SIZE;	//default shared memory size
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

TEEC_Result keyGen(okey *o,storageId sid,const char *keyFileName,uint32_t flags,uint32_t keySize)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = sid;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.params[2].value.a = flags;
	op.params[3].value.a = keySize;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT);

	return TEEC_InvokeCommand(o->session,TA_KEY_GEN_CMD,&op,&o->error);
}

TEEC_Result keyOpen(okey *o,storageId sid,const char *keyFileName,uint32_t flags,uint32_t *keyObj)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = sid;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.params[2].value.a = flags;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_INPUT,TEEC_VALUE_OUTPUT);

	res = TEEC_InvokeCommand(o->session,TA_KEY_OPEN_CMD,&op,&o->error);
	if(res==TEEC_SUCCESS) *keyObj = op.params[3].value.a;
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

TEEC_Result keyInject(okey *o,storageId sid,const char *keyFileName,uint8_t *keyBuffer,size_t keySize,uint32_t flags)
{
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = sid;
	op.params[1].tmpref.buffer = (char*)keyFileName;
	op.params[1].tmpref.size = strlen((const char*)keyFileName);
	op.params[2].tmpref.buffer = keyBuffer ;
	op.params[2].tmpref.size = keySize;
	op.params[3].value.a = flags;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_INPUT);

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

TEEC_Result keyEnumObjectList(okey *o,storageId sid,eObjList **list)
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

	res = fs_start_enum(o->session,enumHandle,sid);
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

TEEC_Result keyAllocOper(okey *o,bool bEnc,uint32_t keyObj,OperationHandle *encOp)
{
	TEEC_Result res;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[1].value.a = TEE_ALG_AES_ECB_NOPAD;	//does not require IV
	op.params[2].value.a = bEnc?MODE_ENCRYPT:MODE_DECRYPT;
	op.params[3].value.a = keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT,TEEC_VALUE_INPUT);

	res = TEEC_InvokeCommand(o->session,TA_KEY_ALLOC_OPER_CMD,&op,&o->error);
	if(res==TEEC_SUCCESS) *encOp = VAL2HANDLE(op.params[0].value.a);
	return res;
}

TEEC_Result keyFreeOper(okey *o,OperationHandle encOp)
{
        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	op.params[0].value.a = (uintptr_t)encOp;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	return TEEC_InvokeCommand(o->session,TA_KEY_FREE_OPER_CMD,&op,&o->error);
}       

TEEC_Result keySetkeyOper(okey *o,OperationHandle encOp,uint32_t keyObj)
{
        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

        op.params[0].value.a = (uintptr_t)encOp;
        op.params[1].value.a = keyObj;
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE);
        return TEEC_InvokeCommand(o->session,TA_KEY_SETKEY_OPER_CMD,&op,&o->error);
}

TEEC_Result cipherInit(okey *o,OperationHandle encOp,uint8_t shMemFactor)
{
	if(shMemFactor==0) return TEE_ERROR_BAD_PARAMETERS;

	TEEC_Result res;
	o->shMemSize = TEE_AES_BLOCK_SIZE*shMemFactor;
	if((res=allocShm(o,&in_shm,o->shMemSize))!=TEEC_SUCCESS) goto cleanup1;
	if((res=allocShm(o,&out_shm,o->shMemSize))!=TEEC_SUCCESS) goto cleanup2;

	set_shm(&in_shm,o->shMemSize);
	set_shm(&out_shm,o->shMemSize);

        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

        op.params[0].value.a = (uintptr_t)encOp;
        op.params[1].tmpref.buffer = 0;
        op.params[1].tmpref.size = 0;
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE,TEEC_NONE);
        return TEEC_InvokeCommand(o->session,TA_CIPHER_INIT_CMD,&op,&o->error);

cleanup2:
	freeShm(&in_shm);
cleanup1:
	return res;
}

TEEC_Result cipherUpdate(okey *o,OperationHandle encOp,uint8_t *inBuf,size_t inBufSize)
{
	if((inBufSize%TEE_AES_BLOCK_SIZE)!=0)
		return TEE_ERROR_NOT_SUPPORTED; 

	if(inBufSize>o->shMemSize)
		return TEE_ERROR_BAD_PARAMETERS;

        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	copy_shm(&in_shm,inBuf,inBufSize);

	op.params[0].value.a = (uintptr_t)encOp;
	op.params[1].memref.parent = &in_shm;
	op.params[1].memref.size = o->shMemSize;
	op.params[2].memref.parent = &out_shm;
	op.params[2].memref.size = o->shMemSize;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_PARTIAL_INPUT,
					 TEEC_MEMREF_PARTIAL_OUTPUT,
					 TEEC_NONE);
	return TEEC_InvokeCommand(o->session,TA_CIPHER_UPDATE_CMD,&op,&o->error);
}

TEEC_Result cipherDoFinal(okey *o,OperationHandle encOp,uint8_t *inBuf,size_t inBufSize)
{
	if((inBufSize%TEE_AES_BLOCK_SIZE)!=0)
		return TEE_ERROR_NOT_SUPPORTED; 

	if(inBufSize>o->shMemSize)
		return TEE_ERROR_BAD_PARAMETERS;

        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;

	copy_shm(&in_shm,inBuf,inBufSize);

	op.params[0].value.a = (uintptr_t)encOp;
	op.params[1].memref.parent = &in_shm;
	op.params[1].memref.size = o->shMemSize;
	op.params[2].memref.parent = &out_shm;
	op.params[2].memref.size = o->shMemSize;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_PARTIAL_INPUT,
					 TEEC_MEMREF_PARTIAL_OUTPUT,
					 TEEC_NONE);
	return TEEC_InvokeCommand(o->session,TA_CIPHER_DO_FINAL_CMD,&op,&o->error);
}

TEEC_SharedMemory *outSharedMemory()
{
	return &out_shm;
}

void cipherClose()
{
	freeShm(&in_shm);
	freeShm(&out_shm);
}
