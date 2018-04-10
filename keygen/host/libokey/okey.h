#ifndef __OKEY__
#define __OKEY__

#include <tee_client_api.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _optee_key_context {
		TEEC_Context *ctx;
		TEEC_Session *session;
		uint32_t returnOrigin;
	} okey;

	TEEC_Result initializeContext(const char *name,okey *o);
	TEEC_Result openSession(okey *o,const TEEC_UUID *destination,uint32_t connectionMethod,
				const void *connectionData,TEEC_Operation *operation);
	void closeSession(okey *o);
	void finalizeContext(okey *o);

#ifdef __cplusplus
}
#endif

#endif
