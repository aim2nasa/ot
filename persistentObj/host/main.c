#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <tee_api_defines.h>
#include <persistentObj_ta.h>

static TEEC_Result fs_create(TEEC_Session *sess,void *id,uint32_t id_size,
			    uint32_t flags, uint32_t attr, void *data,
			    uint32_t data_size, uint32_t *obj,
			    uint32_t storage_id)
{
        TEEC_Operation op = {0};
        TEEC_Result res;
        uint32_t org;

        op.params[0].tmpref.buffer = id;
        op.params[0].tmpref.size = id_size;
        op.params[1].value.a = flags;
        op.params[1].value.b = 0;
        op.params[2].value.a = attr;
        op.params[2].value.b = storage_id;
        op.params[3].tmpref.buffer = data;
        op.params[3].tmpref.size = data_size;

        op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                         TEEC_VALUE_INOUT, TEEC_VALUE_INPUT,
                                         TEEC_MEMREF_TEMP_INPUT);

        res = TEEC_InvokeCommand(sess, TA_PERSISTENTOBJ_CMD_CREATE, &op, &org);

        if (res == TEEC_SUCCESS)
                *obj = op.params[1].value.b;

        return res;
}

static TEEC_Result fs_unlink(TEEC_Session *sess, uint32_t obj)
{
	TEEC_Operation op = {0};
	uint32_t org;

	op.params[0].value.a = obj;

        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
                                         TEEC_NONE, TEEC_NONE);
	return TEEC_InvokeCommand(sess,TA_PERSISTENTOBJ_CMD_UNLINK, &op, &org);
}

static uint8_t objectID[] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07 };
static uint8_t data[] = { 0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x1a,0x1b };

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_PERSISTENTOBJ_UUID;
	uint32_t err_origin;
	uint32_t obj;
	uint32_t storage_id = TEE_STORAGE_PRIVATE;

        printf("TEEC_InitializeContext...\n");
        res = TEEC_InitializeContext(NULL,&ctx);
        if(res!=TEEC_SUCCESS)
                errx(1,"TEEC_InitializeContext failed with code 0x%x",res);
        printf("TEEC_InitializeContext ok\n");

        printf("TEEC_OpenSession...\n");
        res = TEEC_OpenSession(&ctx,&sess,&uuid,TEEC_LOGIN_PUBLIC,NULL,NULL,&err_origin);
        if(res!=TEEC_SUCCESS)
                errx(1,"TEEC_OpenSession failed with code 0x%x origin 0x%x",res,err_origin);
        printf("TEEC_OpenSession ok\n");

        printf("Creating in TA...\n");
	res = fs_create(&sess,objectID,sizeof(objectID),
		TEE_DATA_FLAG_ACCESS_WRITE|
		TEE_DATA_FLAG_ACCESS_WRITE_META,
		0,data,sizeof(data),&obj,storage_id);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_create failed with code 0x%x",res);
        printf("Created in TA, obj=0x%x\n",obj);

        printf("Unlink...\n");
	res = fs_unlink(&sess,obj);
        if(res!=TEEC_SUCCESS)
                errx(1,"fs_unlink failed with code 0x%x",res);
        printf("Unlink done\n");

        printf("TEEC_FinalizeContext...\n");
        TEEC_FinalizeContext(&ctx);
        printf("TEEC_FinalizeContext ok\n");

	printf("PersitentObj end\n");
	return 0;
}
