#include <err.h>
#include <stdio.h>
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
	okey o;
	eObjList *list = NULL;

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

	res = keyEnumObjectList(&o,TEE_STORAGE_PRIVATE,&list);
        if(res!=TEEC_SUCCESS)
                errx(1,"keyEnumObjectList failed with code 0x%x",res);

	printf("list=%p\n",list);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyEnum end\n");
	return 0;
}
