#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <keygen_ta.h>
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
	TEEC_UUID uuid = TA_KEYGEN_UUID;
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

	print("initializeContext...\n");
	res = initializeContext(NULL,&o);
	if(res!=TEEC_SUCCESS)
		errx(1,"initializeContext failed with code 0x%x",res);
	print("initializeContext ok\n");

	print("openSession...\n");
	res = openSession(&o,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL);
	if(res!=TEEC_SUCCESS)
		errx(1,"openSession failed with code 0x%x origin 0x%x",res,o.error);
	print("openSession ok\n");

	res = keyGen(&o,TEE_STORAGE_PRIVATE,(char*)key_filename);
	if(res!=TEEC_SUCCESS)
		errx(1,"keyGen failed with code 0x%x origin 0x%x",res,o.error);
	printf("key generated in file:%s\n",key_filename);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyGen end\n");
	return 0;
}
