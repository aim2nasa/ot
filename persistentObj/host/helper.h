#include <tee_client_api.h>

TEEC_Result fs_create(TEEC_Session *sess,void *id,uint32_t id_size,
                      uint32_t flags, uint32_t attr, void *data,
                      uint32_t data_size, uint32_t *obj,
                      uint32_t storage_id);

TEEC_Result fs_unlink(TEEC_Session *sess, uint32_t obj);

TEEC_Result fs_open(TEEC_Session *sess, void *id, uint32_t id_size,
		    uint32_t flags, uint32_t *obj, uint32_t storage_id);

TEEC_Result fs_close(TEEC_Session *sess, uint32_t obj);
