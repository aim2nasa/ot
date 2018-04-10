#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <keygen_ta.h>
#include <string.h>
#include <okey.h>

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
	TEEC_UUID uuid = TA_KEYGEN_UUID;
	uint32_t err_origin;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint8_t key_filename[256]={ 0 };
	okey o;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
	}else{
		printf("tool to self-generate AES key and stores it in trustzone(secure storage)\n");
                printf("usage: keygen <key filename in TEE>\n");
                return 1;
	}

	print("key filename:%s\n",key_filename);

	print("TEEC_InitializeContext...\n");
	res = initializeContext(NULL,&o);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InitializeContext failed with code 0x%x",res);
	print("TEEC_InitializeContext ok\n");

	print("TEEC_OpenSession...\n");
	res = openSession(&o,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_OpenSession failed with code 0x%x origin 0x%x",res,o.returnOrigin);
	print("TEEC_OpenSession ok\n");

	print("Invoking TA...\n");
	op.params[0].value.a = TEE_STORAGE_PRIVATE;
	op.params[1].tmpref.buffer = key_filename;
	op.params[1].tmpref.size = strlen((const char*)key_filename);
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE,TEEC_NONE);

	res = TEEC_InvokeCommand(o.session,TA_KEY_GEN_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);
	print("TA Invoked\n");

	printf("key generated in file:%s\n",key_filename);

	print("TEEC_FinalizeContext...\n");
	finalizeContext(&o);
	print("TEEC_FinalizeContext ok\n");

	print("KeyGen end\n");
	return 0;
}
