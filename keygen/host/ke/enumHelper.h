#ifndef ENUM_HELPER_H
#define ENUM_HELPER_H

#include <tee_client_api.h>

TEEC_Result fs_alloc_enum(TEEC_Session *sess, uint32_t *e);
TEEC_Result fs_reset_enum(TEEC_Session *sess, uint32_t e);
TEEC_Result fs_free_enum(TEEC_Session *sess, uint32_t e);
TEEC_Result fs_start_enum(TEEC_Session *sess, uint32_t e,uint32_t storage_id);
TEEC_Result fs_next_enum(TEEC_Session *sess, uint32_t e, void *obj_info,
			 size_t *info_size, void *id, uint32_t *id_size);

#endif
