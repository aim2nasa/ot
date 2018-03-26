#include <err.h>
#include <stdio.h>
#include <tee_api_types.h>
#include <tee_client_api.h>
#include <keygen_ta.h>
#include <string.h>
#include <assert.h>

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

TEEC_Result get_object_buffer_attribute(TEEC_Session *s,
                                                     TEE_ObjectHandle o,
                                                     uint32_t attr_id,
                                                     void *buf, size_t *blen)
{
        TEEC_Result res;
        TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
        uint32_t ret_orig;

        assert((uintptr_t)o <= UINT32_MAX);
        op.params[0].value.a = (uint32_t)(uintptr_t)o;
        op.params[0].value.b = attr_id;

        op.params[1].tmpref.buffer = buf;
        op.params[1].tmpref.size = *blen;

        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
                                         TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE,
                                         TEEC_NONE);

        res = TEEC_InvokeCommand(s, TA_KEY_GET_OBJECT_BUFFER_ATTRIBUTE_CMD,
                                 &op, &ret_orig);

        if (res == TEEC_SUCCESS)
                *blen = op.params[1].tmpref.size;

        return res;
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_KEYGEN_UUID;
	uint32_t err_origin;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint8_t key_filename[256]={ 0 },key_buffer[1024]={ 0 };
	TEE_ObjectHandle keyObj = (TEE_ObjectHandle)NULL;
	size_t i,key_size;
	FILE *fp;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
	}else
		memcpy(key_filename,"test.key",strlen("test.key"));

	printf("key filename:%s\n",key_filename);
	if(argc>2) printf("key dump file:%s\n",argv[2]);

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
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_MEMREF_TEMP_INPUT,TEEC_VALUE_OUTPUT,TEEC_NONE);

	res = TEEC_InvokeCommand(&sess,TA_KEY_OPEN_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);
	
	keyObj = (TEE_ObjectHandle)(uintptr_t)op.params[2].value.a;
	print("TA Invoked\n");

	printf("file open successful:%s,handle:%p\n",key_filename,keyObj);

	key_size = sizeof(key_buffer);
	res = get_object_buffer_attribute(&sess,keyObj,TEE_ATTR_SECRET_VALUE,key_buffer,&key_size);
	if(res!=TEEC_SUCCESS){
		errx(1,"get_object_buffer_attribute failed with code 0x%x",res);
	}
	if(argc>2){
		if((fp=fopen(argv[2],"wb"))!=NULL){
			int nWrite = fwrite(key_buffer,1,key_size,fp);
			assert(nWrite==key_size);
			fclose(fp);
		}else{
			printf("fopen failure:%s\n",argv[2]);
		}
	}
	printf("Obtained keySize=%zd\n",key_size);
	for(i=0;i<key_size;i++) printf("%x ",key_buffer[i]);
	printf("\n");
	
	op.params[0].value.a = (uint32_t)(uintptr_t)keyObj;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,TEEC_NONE,TEEC_NONE,TEEC_NONE);
	res = TEEC_InvokeCommand(&sess,TA_KEY_CLOSE_CMD,&op,&err_origin);
	if(res!=TEEC_SUCCESS)
		errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);

	printf("file close successful:%s,handle:%p\n",key_filename,keyObj);

	print("TEEC_FinalizeContext...\n");
	TEEC_FinalizeContext(&ctx);
	print("TEEC_FinalizeContext ok\n");

	print("KeyOpen end\n");
	return 0;
}
