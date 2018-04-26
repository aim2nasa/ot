#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
	uint32_t flags;
	uint32_t keySize;

	if(argc>2){
		if(strlen(argv[1])>=sizeof(key_filename))
			errx(1,"key filename is over the buffer limit(%zd)\n",sizeof(key_filename));

		memcpy(key_filename,argv[1],strlen(argv[1]));
		keySize = atoi(argv[2]);
	}else{
		printf("tool to self-generate AES key and stores it in trustzone(secure storage)\n");
                printf("usage: keygen <key filename in TEE> <keySize(bits)>\n");
                printf("       supported key sizes(in bits) are 128,192,256\n");
                return 1;
	}

	print("key filename:%s\n",key_filename);
	print("key size:%d\n",keySize);

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

	flags = TEE_DATA_FLAG_ACCESS_WRITE_META	|
		TEE_DATA_FLAG_ACCESS_READ	|
		TEE_DATA_FLAG_SHARE_READ	|
		TEE_DATA_FLAG_ACCESS_WRITE	|
		TEE_DATA_FLAG_SHARE_WRITE;
	res = keyGen(&o,TEE_STORAGE_PRIVATE,(char*)key_filename,flags,keySize);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyGen failed with code 0x%x origin 0x%x flags:0x%x",res,o.error,flags);
	printf("key generated in file:%s flags:0x%x keySize:%d\n",key_filename,flags,keySize);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyGen end\n");
	return 0;
}
