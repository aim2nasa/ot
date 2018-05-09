#include <err.h>
#include <stdio.h>
#include <string.h>
#include <okey.h>

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
	uint8_t key_filename[256]={ 0 };
	okey o;
	uint32_t keyObj=0;
	uint32_t flags;

	if(argc>1){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
	}else{
		printf("tool to delete key stored in trustzone\n");
		printf("usage: keydel <key filename in TEE>\n");
		return 1;
	}

	print("key filename:%s\n",key_filename);

	print("initializeContext...\n");
	res = initializeContext(NULL,&o);
	if(res!=TEEC_SUCCESS)
		errx(1,"initializeContext failed with code 0x%x",res);
	print("initializeContext ok\n");

	print("openSession...\n");
	res = openSession(&o,TEEC_LOGIN_PUBLIC,NULL,NULL);
	if(res!=TEEC_SUCCESS)
		errx(1,"openSession failed with code 0x%x origin 0x%x",res,o.error);
	print("openSession ok\n");

        flags = TEE_DATA_FLAG_ACCESS_WRITE_META |
                TEE_DATA_FLAG_ACCESS_READ       |
                TEE_DATA_FLAG_SHARE_READ        |
                TEE_DATA_FLAG_ACCESS_WRITE      |
                TEE_DATA_FLAG_SHARE_WRITE;
	res = keyOpen(&o,private,(char*)key_filename,flags,&keyObj);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyOpen failed with code 0x%x origin 0x%x flags:0x%x",res,o.error,flags);
	
	printf("file open successful:%s,handle:0x%x flags:0x%x\n",key_filename,keyObj,flags);

	res = keyUnlink(&o,keyObj);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyUnlink failed with code 0x%x origin 0x%x",res,o.error);

	printf("file unlink successful:%s,handle:0x%x\n",key_filename,keyObj);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyOpen end\n");
	return 0;
}
