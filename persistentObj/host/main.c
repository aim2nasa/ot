#include <err.h>
#include <stdio.h>
#include <tee_client_api.h>
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

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_PERSISTENTOBJ_UUID;
	uint32_t err_origin;

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

        printf("Invoking TA...\n");
        res = TEEC_InvokeCommand(&sess,TA_PERSISTENTOBJ_CMD_CREATE,NULL,&err_origin);
        if(res!=TEEC_SUCCESS)
                errx(1,"TEEC_InvokeCommand failed with code 0x%x origin 0x%x",res,err_origin);
        printf("TA Invoked\n");

        printf("TEEC_FinalizeContext...\n");
        TEEC_FinalizeContext(&ctx);
        printf("TEEC_FinalizeContext ok\n");

	printf("PersitentObj end\n");
	return 0;
}
