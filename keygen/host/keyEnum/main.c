#include <err.h>
#include <stdio.h>
#include <string.h>
#include <okey.h>

#define TEE_OBJECT_ID_MAX_LEN              64

int print(const char *format,...)
{
#ifdef DEBUG
	return printf(format);
#else
	return 0;
#endif
}

void displayList(eObjList *list)
{
	int i=0;
	char id[TEE_OBJECT_ID_MAX_LEN+1]={0};
	eObjList *cur=list;

	while(cur){
		memcpy(id,cur->object->id,cur->object->idSize);
		id[cur->object->idSize]=0;

		printf("[%d] %s\n",i++,id);

		if(cur->next)
			cur = cur->next;
		else
			cur = NULL;
	}
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

	res = keyEnumObjectList(&o,private,&list);
        if(res!=TEEC_SUCCESS)
                errx(1,"keyEnumObjectList failed with code 0x%x",res);

	printf("list=%p\n",list);
	displayList(list);
	printf("%d eliments in list(%p) have freed\n",keyFreeEnumObjectList(list),list);

	print("finalizeContext...\n");
	finalizeContext(&o);
	print("finalizeContext ok\n");

	print("KeyEnum end\n");
	return 0;
}
