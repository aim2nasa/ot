#ifndef __OKEY__
#define __OKEY__

#include <tee_client_api.h>

#define TEE_STORAGE_PRIVATE		0x00000001

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _optee_key_context {
		TEEC_Context *ctx;
		TEEC_Session *session;
		uint32_t error;
	} okey;

	TEEC_Result initializeContext(const char *name,okey *o);
	TEEC_Result openSession(okey *o,uint32_t connectionMethod,
				const void *connectionData,TEEC_Operation *operation);
	TEEC_Result keyGen(okey *o,uint32_t storageId,const char *keyFileName);
	TEEC_Result keyOpen(okey *o,uint32_t storageId,const char *keyFileName,uint32_t *keyObj);
	TEEC_Result keyInject(okey *o,uint32_t storageId,const char *keyFileName,uint8_t *keyBuffer,size_t keySize);
	TEEC_Result keyGetObjectBufferAttribute(okey *o,uint32_t keyObj,uint32_t attrId,void *buffer,size_t *bufferSize);
	TEEC_Result keyUnlink(okey *o,uint32_t keyObj);
	void closeSession(okey *o);
	void finalizeContext(okey *o);

#ifdef __cplusplus
}
#endif

#endif
