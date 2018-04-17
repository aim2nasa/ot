#ifndef __OKEY__
#define __OKEY__

#include <tee_client_api.h>
#include <tee_api_defines.h>
#include <tee_api_types.h>

#define TEE_STORAGE_PRIVATE		0x00000001

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _optee_key_context {
		TEEC_Context *ctx;
		TEEC_Session *session;
		uint32_t error;
	} okey;

	typedef struct _enum_object {
		uint8_t *id;
		uint32_t idSize;
	} eObj;

	typedef struct _enum_object_list {
		eObj *object;
		struct _enum_object_list *next;
	} eObjList;

	TEEC_Result initializeContext(const char *name,okey *o);
	TEEC_Result openSession(okey *o,uint32_t connectionMethod,
				const void *connectionData,TEEC_Operation *operation);
	TEEC_Result keyGen(okey *o,uint32_t storageId,const char *keyFileName);
	TEEC_Result keyOpen(okey *o,uint32_t storageId,const char *keyFileName,uint32_t *keyObj);
	TEEC_Result keyInject(okey *o,uint32_t storageId,const char *keyFileName,uint8_t *keyBuffer,size_t keySize);
	TEEC_Result keyGetObjectBufferAttribute(okey *o,uint32_t keyObj,uint32_t attrId,void *buffer,size_t *bufferSize);
	TEEC_Result keyEnumObjectList(okey *o,uint32_t storageId,eObjList **list);
	int keyFreeEnumObjectList(eObjList *list);
	TEEC_Result keyAllocOper(okey *o,bool bEnc,size_t keySize,TEE_OperationHandle *encOp);
	TEEC_Result keyFreeOper(okey *o,TEE_OperationHandle encOp);
	TEEC_Result keySetkeyOper(okey *o,TEE_OperationHandle encOp,uint32_t keyObj);
	TEEC_Result keyClose(okey *o,uint32_t keyObj);
	TEEC_Result keyUnlink(okey *o,uint32_t keyObj);
	TEEC_Result allocShm(okey *o,TEEC_SharedMemory *shm,size_t size);
	void freeShm(TEEC_SharedMemory *shm);
	TEEC_Result cipherInit(okey *o,TEE_OperationHandle encOp);
	TEEC_Result cipherUpdate(okey *o,TEE_OperationHandle encOp,uint8_t *inBuf,size_t inBufSize);
	void closeSession(okey *o);
	void finalizeContext(okey *o);

#ifdef __cplusplus
}
#endif

#endif
