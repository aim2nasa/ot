#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

#include <persistentObj_ta.h>

#define TA_UUID				TA_PERSISTENTOBJ_UUID

#define TA_FLAGS			TA_FLAG_EXEC_DDR
#define TA_STACK_SIZE			(2 * 1024)
#define TA_DATA_SIZE			(32 * 1024)

/* Extra properties (give a version id and a string name) */
#define TA_CURRENT_TA_EXT_PROPERTIES \
    { "gp.ta.description", USER_TA_PROP_TYPE_STRING, \
        "Test of PersistentObj ref.6001 test from optee_test" }, \
    { "gp.ta.version", USER_TA_PROP_TYPE_U32, &(const uint32_t){ 0x0010 } }

#endif

