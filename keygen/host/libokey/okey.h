#ifndef __OKEY__
#define __OKEY__

#include <tee_client_api.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _optee_key_context {
		TEEC_Context *ctx;
	} okey;

	TEEC_Result initializeContext(const char *name,okey *o);
	void finalizeContext(okey *o);

#ifdef __cplusplus
}
#endif

#endif
