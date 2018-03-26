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
	uint8_t key_filename[256]={ 0 },stored_key_filename[256]={ 0 };
	uint8_t key_buffer[32]={0};
	size_t i,key_size;
	FILE *fp;

	if(argc>2){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		if(strlen(argv[2])>=sizeof(stored_key_filename))
			errx(1,"stored key filename is over the buffer limit(%zd)\n",sizeof(stored_key_filename));
		memcpy(key_filename,argv[1],strlen(argv[1]));
		memcpy(stored_key_filename,argv[2],strlen(argv[2]));
	}else{
		printf("usage: keyinj <key filename in TEE> <key filename stored in REE>\n");
		return 1;
	}

	print("key filename in TEE:%s\n",key_filename);
	print("key filename stored in REE:%s\n",key_filename);

	if((fp=fopen(argv[2],"rb"))!=NULL){
		key_size = 8*fread(key_buffer,1,sizeof(key_buffer),fp);
		fclose(fp);
	}else{
		printf("fopen failure:%s\n",argv[2]);
	}
	printf("Obtained keySize=%zd\n",key_size);
	for(i=0;i<key_size/8;i++) printf("%x ",key_buffer[i]);
	printf("\n");

	print("TEEC_InitializeContext...\n");
	res = TEEC_InitializeContext(NULL,&ctx);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InitializeContext failed with code 0x%x",res);
	print("TEEC_InitializeContext ok\n");

	print("TEEC_OpenSession...\n");
	res = TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_OpenSession failed with code 0x%x origin 0x%x",res,err_origin);
	print("TEEC_OpenSession ok\n");

	print("Invoking TA...\n");
	op.params[0].value.a = TEE_STORAGE_PRIVATE;
	op.params[1].tmpref.buffer = key_filename;
	op.params[1].tmpref.size = strlen((const char*)key_filename);
	op.params[2].tmpref.buffer = key_buffer;
	op.params[2].tmpref.size = key_size;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_NONE);

	res = TEEC_InvokeCommand(&sess,TA_KEY_INJECT_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);
	print("TA Invoked\n");

	printf("key(%s) is injected into file(%s) in TEE\n",stored_key_filename,key_filename);

	print("TEEC_FinalizeContext...\n");
	TEEC_FinalizeContext(&ctx);
	print("TEEC_FinalizeContext ok\n");

	print("KeyInj end\n");
	return 0;
}
