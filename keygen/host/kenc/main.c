#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <keygen_ta.h>
#include <string.h>

#define TEEC_OPERATION_INITIALIZER	{ 0 }
#define TEE_STORAGE_PRIVATE		0x00000001

int print(const char *format,...)
{
#ifdef DEBUG
	return printf(format);
#else
	return 0;
#endif
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_KEYGEN_UUID;
	uint32_t err_origin;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint8_t key_filename[256]={ 0 };
	uint32_t keyObj=0;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
	}else{
		errx(1,"usage: kenc <keyfile>");
	}

	res = TEEC_InitializeContext(NULL,&ctx);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InitializeContext failed with code 0x%x",res);

	res = TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_OpenSession failed with code 0x%x origin 0x%x",res,err_origin);

	op.params[0].value.a = TEE_STORAGE_PRIVATE;
	op.params[1].tmpref.buffer = key_filename;
	op.params[1].tmpref.size = strlen((const char*)key_filename);
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_OUTPUT,TEEC_NONE);

	res = TEEC_InvokeCommand(&sess,TA_KEY_OPEN_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);
	
	keyObj = op.params[2].value.a;	

	printf("key obtained:%s,handle:%u\n",key_filename,keyObj);
	
	op.params[0].value.a = keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	res = TEEC_InvokeCommand(&sess,TA_KEY_CLOSE_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);

	TEEC_FinalizeContext(&ctx);
	print("KeyEnc end\n");
	return 0;
}
