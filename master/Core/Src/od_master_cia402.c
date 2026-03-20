#if !LELY_NO_MALLOC
#error Static object dictionaries are only supported when dynamic memory allocation is disabled.
#endif

#include <lely/co/detail/dev.h>
#include <lely/co/detail/obj.h>
#include <lely/util/cmp.h>

#define CO_DEV_STRING(s) s

static co_dev_t od_master = {
	.netid = 0,
	.id = 1,
	.tree = { .cmp = &uint16_cmp },
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING(""),
	.vendor_name = CO_DEV_STRING("N7 Space Sp. z o.o."),
#endif
	.vendor_id = 0x004E3753,
#if !LELY_NO_CO_OBJ_NAME
	.product_name = CO_DEV_STRING(""),
#endif
	.product_code = 0x00000000,
	.revision = 0x00000000,
#if !LELY_NO_CO_OBJ_NAME
	.order_code = CO_DEV_STRING(""),
#endif
	.baud = 0 | CO_BAUD_10 | CO_BAUD_20 | CO_BAUD_50 | CO_BAUD_125 | CO_BAUD_250 | CO_BAUD_500 | CO_BAUD_800 | CO_BAUD_1000,
	.rate = 1000,
	.lss = 0,
	.dummy = 0x00000000
};

static struct {
	co_unsigned32_t sub0;
} od_master_1000_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_master_1000 = {
	.node = { .key = &od_master_1000.idx },
	.idx = 0x1000,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Device type"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1000_val,
	.size = sizeof(od_master_1000_val)
};

static co_sub_t od_master_1000sub0 = {
	.node = { .key = &od_master_1000sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Device type"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1000_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
} od_master_1001_val = {
	.sub0 = 0x00,
};

static co_obj_t od_master_1001 = {
	.node = { .key = &od_master_1001.idx },
	.idx = 0x1001,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Error register"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1001_val,
	.size = sizeof(od_master_1001_val)
};

static co_sub_t od_master_1001sub0 = {
	.node = { .key = &od_master_1001sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Error register"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_master_1001_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_master_1005_val = {
	.sub0 = 0x00000080,
};

static co_obj_t od_master_1005 = {
	.node = { .key = &od_master_1005.idx },
	.idx = 0x1005,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID SYNC"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1005_val,
	.size = sizeof(od_master_1005_val)
};

static co_sub_t od_master_1005sub0 = {
	.node = { .key = &od_master_1005sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID SYNC"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000080 },
#endif
	.val = &od_master_1005_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_master_1006_val = {
	.sub0 = 0x000F4240,
};

static co_obj_t od_master_1006 = {
	.node = { .key = &od_master_1006.idx },
	.idx = 0x1006,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Communication cycle period"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1006_val,
	.size = sizeof(od_master_1006_val)
};

static co_sub_t od_master_1006sub0 = {
	.node = { .key = &od_master_1006sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Communication cycle period"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x000F4240 },
#endif
	.val = &od_master_1006_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
} od_master_1016_val = {
	.sub0 = 0x01,
	.sub1 = 0x00020226,
};

static co_obj_t od_master_1016 = {
	.node = { .key = &od_master_1016.idx },
	.idx = 0x1016,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Consumer heartbeat time"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1016_val,
	.size = sizeof(od_master_1016_val)
};

static co_sub_t od_master_1016sub0 = {
	.node = { .key = &od_master_1016sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Highest sub-index supported"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x01 },
#endif
	.val = &od_master_1016_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1016sub1 = {
	.node = { .key = &od_master_1016sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Consumer heartbeat time"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00020226 },
#endif
	.val = &od_master_1016_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned16_t sub0;
} od_master_1017_val = {
	.sub0 = 0x01F4,
};

static co_obj_t od_master_1017 = {
	.node = { .key = &od_master_1017.idx },
	.idx = 0x1017,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Producer heartbeat time"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1017_val,
	.size = sizeof(od_master_1017_val)
};

static co_sub_t od_master_1017sub0 = {
	.node = { .key = &od_master_1017sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Producer heartbeat time"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x01F4 },
#endif
	.val = &od_master_1017_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
	co_unsigned32_t sub3;
	co_unsigned32_t sub4;
} od_master_1018_val = {
	.sub0 = 0x04,
	.sub1 = 0x004E3753,
	.sub2 = 0x00000000,
	.sub3 = 0x00000000,
	.sub4 = 0x00000000,
};

static co_obj_t od_master_1018 = {
	.node = { .key = &od_master_1018.idx },
	.idx = 0x1018,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Identity object"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1018_val,
	.size = sizeof(od_master_1018_val)
};

static co_sub_t od_master_1018sub0 = {
	.node = { .key = &od_master_1018sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Highest sub-index supported"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x04 },
#endif
	.val = &od_master_1018_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1018sub1 = {
	.node = { .key = &od_master_1018sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Vendor-ID"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x004E3753 },
#endif
	.val = &od_master_1018_val.sub1,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1018sub2 = {
	.node = { .key = &od_master_1018sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Product code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1018_val.sub2,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1018sub3 = {
	.node = { .key = &od_master_1018sub3.subidx },
	.subidx = 0x03,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Revision number"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1018_val.sub3,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1018sub4 = {
	.node = { .key = &od_master_1018sub4.subidx },
	.subidx = 0x04,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Serial number"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1018_val.sub4,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
} od_master_1019_val = {
	.sub0 = 0x10,
};

static co_obj_t od_master_1019 = {
	.node = { .key = &od_master_1019.idx },
	.idx = 0x1019,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Synchronous counter overflow value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1019_val,
	.size = sizeof(od_master_1019_val)
};

static co_sub_t od_master_1019sub0 = {
	.node = { .key = &od_master_1019sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Synchronous counter overflow value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x10 },
#endif
	.val = &od_master_1019_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
	co_unsigned8_t sub3;
} od_master_1280_val = {
	.sub0 = 0x03,
	.sub1 = 0x00000602,
	.sub2 = 0x00000582,
	.sub3 = 0x02,
};

static co_obj_t od_master_1280 = {
	.node = { .key = &od_master_1280.idx },
	.idx = 0x1280,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("SDO client parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1280_val,
	.size = sizeof(od_master_1280_val)
};

static co_sub_t od_master_1280sub0 = {
	.node = { .key = &od_master_1280sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Highest sub-index supported"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x03 },
#endif
	.val = &od_master_1280_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1280sub1 = {
	.node = { .key = &od_master_1280sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID client to server (tx)"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000602 },
#endif
	.val = &od_master_1280_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1280sub2 = {
	.node = { .key = &od_master_1280sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID server to client (rx)"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000582 },
#endif
	.val = &od_master_1280_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1280sub3 = {
	.node = { .key = &od_master_1280sub3.subidx },
	.subidx = 0x03,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Node-ID of the SDO server"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_master_1280_val.sub3,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned8_t sub2;
} od_master_1400_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000182,
	.sub2 = 0x01,
};

static co_obj_t od_master_1400 = {
	.node = { .key = &od_master_1400.idx },
	.idx = 0x1400,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("RPDO1 communication parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1400_val,
	.size = sizeof(od_master_1400_val)
};

static co_sub_t od_master_1400sub0 = {
	.node = { .key = &od_master_1400sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Highest sub-index supported"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_master_1400_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1400sub1 = {
	.node = { .key = &od_master_1400sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID used by RPDO"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000182 },
#endif
	.val = &od_master_1400_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1400sub2 = {
	.node = { .key = &od_master_1400sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Transmission type"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x01 },
#endif
	.val = &od_master_1400_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
} od_master_1600_val = {
	.sub0 = 0x02,
	.sub1 = 0x60410010,
	.sub2 = 0x60640020,
};

static co_obj_t od_master_1600 = {
	.node = { .key = &od_master_1600.idx },
	.idx = 0x1600,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("RPDO1 mapping parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1600_val,
	.size = sizeof(od_master_1600_val)
};

static co_sub_t od_master_1600sub0 = {
	.node = { .key = &od_master_1600sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NrOfObjects"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_master_1600_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1600sub1 = {
	.node = { .key = &od_master_1600sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("RPDO1 mapping parameter1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1600_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1600sub2 = {
	.node = { .key = &od_master_1600sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("RPDO1 mapping parameter2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1600_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned8_t sub2;
} od_master_1800_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000181,
	.sub2 = 0x01,
};

static co_obj_t od_master_1800 = {
	.node = { .key = &od_master_1800.idx },
	.idx = 0x1800,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO1 communication parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1800_val,
	.size = sizeof(od_master_1800_val)
};

static co_sub_t od_master_1800sub0 = {
	.node = { .key = &od_master_1800sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Highest sub-index supported"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_master_1800_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1800sub1 = {
	.node = { .key = &od_master_1800sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID used by TPDO"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000181 },
#endif
	.val = &od_master_1800_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0 | CO_OBJ_FLAGS_DEF_NODEID | CO_OBJ_FLAGS_VAL_NODEID,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1800sub2 = {
	.node = { .key = &od_master_1800sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Transmission type"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x01 },
#endif
	.val = &od_master_1800_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
} od_master_1A00_val = {
	.sub0 = 0x02,
	.sub1 = 0x60400010,
	.sub2 = 0x607A0020,
};

static co_obj_t od_master_1A00 = {
	.node = { .key = &od_master_1A00.idx },
	.idx = 0x1A00,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO1 mapping parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1A00_val,
	.size = sizeof(od_master_1A00_val)
};

static co_sub_t od_master_1A00sub0 = {
	.node = { .key = &od_master_1A00sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NrOfObjects"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_master_1A00_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1A00sub1 = {
	.node = { .key = &od_master_1A00sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO1 mapping parameter1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1A00_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1A00sub2 = {
	.node = { .key = &od_master_1A00sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO1 mapping parameter2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1A00_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_master_1F80_val = {
	.sub0 = 0x00000001,
};

static co_obj_t od_master_1F80 = {
	.node = { .key = &od_master_1F80.idx },
	.idx = 0x1F80,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NMT startup"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F80_val,
	.size = sizeof(od_master_1F80_val)
};

static co_sub_t od_master_1F80sub0 = {
	.node = { .key = &od_master_1F80sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NMT startup"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F80_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0 | CO_OBJ_FLAGS_PARAMETER_VALUE,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
} od_master_1F81_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x00000001,
};

static co_obj_t od_master_1F81 = {
	.node = { .key = &od_master_1F81.idx },
	.idx = 0x1F81,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NMT slave assignment"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F81_val,
	.size = sizeof(od_master_1F81_val)
};

static co_sub_t od_master_1F81sub0 = {
	.node = { .key = &od_master_1F81sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NrOfObjects"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_master_1F81_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F81sub1 = {
	.node = { .key = &od_master_1F81sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NMT slave assignment1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F81_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F81sub2 = {
	.node = { .key = &od_master_1F81sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NMT slave assignment2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F81_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned8_t sub0;
	co_unsigned8_t sub1;
} od_master_2000_val = {
	.sub0 = 0x01,
	.sub1 = 0x00,
};

static co_obj_t od_master_2000 = {
	.node = { .key = &od_master_2000.idx },
	.idx = 0x2000,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Redundancy object"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_2000_val,
	.size = sizeof(od_master_2000_val)
};

static co_sub_t od_master_2000sub0 = {
	.node = { .key = &od_master_2000sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Highest sub-index supported"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x01 },
#endif
	.val = &od_master_2000_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_2000sub1 = {
	.node = { .key = &od_master_2000sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Bdefault"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_master_2000_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned16_t sub0;
} od_master_6040_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_master_6040 = {
	.node = { .key = &od_master_6040.idx },
	.idx = 0x6040,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Controlword"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6040_val,
	.size = sizeof(od_master_6040_val)
};

static co_sub_t od_master_6040sub0 = {
	.node = { .key = &od_master_6040sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Controlword"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x0000 },
#endif
	.val = &od_master_6040_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned16_t sub0;
} od_master_6041_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_master_6041 = {
	.node = { .key = &od_master_6041.idx },
	.idx = 0x6041,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Statusword"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6041_val,
	.size = sizeof(od_master_6041_val)
};

static co_sub_t od_master_6041sub0 = {
	.node = { .key = &od_master_6041sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Statusword"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x0000 },
#endif
	.val = &od_master_6041_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer8_t sub0;
} od_master_6060_val = {
	.sub0 = 8,
};

static co_obj_t od_master_6060 = {
	.node = { .key = &od_master_6060.idx },
	.idx = 0x6060,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Modes of operation"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6060_val,
	.size = sizeof(od_master_6060_val)
};

static co_sub_t od_master_6060sub0 = {
	.node = { .key = &od_master_6060sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Modes of operation"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i8 = CO_INTEGER8_MIN },
	.max = { .i8 = CO_INTEGER8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i8 = 8 },
#endif
	.val = &od_master_6060_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer8_t sub0;
} od_master_6061_val = {
	.sub0 = 0,
};

static co_obj_t od_master_6061 = {
	.node = { .key = &od_master_6061.idx },
	.idx = 0x6061,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Modes of operation display"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6061_val,
	.size = sizeof(od_master_6061_val)
};

static co_sub_t od_master_6061sub0 = {
	.node = { .key = &od_master_6061sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Modes of operation display"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i8 = CO_INTEGER8_MIN },
	.max = { .i8 = CO_INTEGER8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i8 = 0 },
#endif
	.val = &od_master_6061_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_master_6064_val = {
	.sub0 = 0,
};

static co_obj_t od_master_6064 = {
	.node = { .key = &od_master_6064.idx },
	.idx = 0x6064,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position actual value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6064_val,
	.size = sizeof(od_master_6064_val)
};

static co_sub_t od_master_6064sub0 = {
	.node = { .key = &od_master_6064sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position actual value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_master_6064_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_master_6065_val = {
	.sub0 = 0x00000001,
};

static co_obj_t od_master_6065 = {
	.node = { .key = &od_master_6065.idx },
	.idx = 0x6065,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Following error window"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6065_val,
	.size = sizeof(od_master_6065_val)
};

static co_sub_t od_master_6065sub0 = {
	.node = { .key = &od_master_6065sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Following error window"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000001 },
#endif
	.val = &od_master_6065_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_master_6071_val = {
	.sub0 = 0,
};

static co_obj_t od_master_6071 = {
	.node = { .key = &od_master_6071.idx },
	.idx = 0x6071,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target torque"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6071_val,
	.size = sizeof(od_master_6071_val)
};

static co_sub_t od_master_6071sub0 = {
	.node = { .key = &od_master_6071sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target torque"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_master_6071_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned16_t sub0;
} od_master_6072_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_master_6072 = {
	.node = { .key = &od_master_6072.idx },
	.idx = 0x6072,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max torque"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6072_val,
	.size = sizeof(od_master_6072_val)
};

static co_sub_t od_master_6072sub0 = {
	.node = { .key = &od_master_6072sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max torque"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x0000 },
#endif
	.val = &od_master_6072_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_master_6077_val = {
	.sub0 = 0,
};

static co_obj_t od_master_6077 = {
	.node = { .key = &od_master_6077.idx },
	.idx = 0x6077,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque actual value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6077_val,
	.size = sizeof(od_master_6077_val)
};

static co_sub_t od_master_6077sub0 = {
	.node = { .key = &od_master_6077sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque actual value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_master_6077_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_master_607A_val = {
	.sub0 = 0,
};

static co_obj_t od_master_607A = {
	.node = { .key = &od_master_607A.idx },
	.idx = 0x607A,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target position"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_607A_val,
	.size = sizeof(od_master_607A_val)
};

static co_sub_t od_master_607Asub0 = {
	.node = { .key = &od_master_607Asub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target position"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_master_607A_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_master_6081_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_master_6081 = {
	.node = { .key = &od_master_6081.idx },
	.idx = 0x6081,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile velocity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6081_val,
	.size = sizeof(od_master_6081_val)
};

static co_sub_t od_master_6081sub0 = {
	.node = { .key = &od_master_6081sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile velocity"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_6081_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_master_60B1_val = {
	.sub0 = 0,
};

static co_obj_t od_master_60B1 = {
	.node = { .key = &od_master_60B1.idx },
	.idx = 0x60B1,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Velocity offset"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_60B1_val,
	.size = sizeof(od_master_60B1_val)
};

static co_sub_t od_master_60B1sub0 = {
	.node = { .key = &od_master_60B1sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Velocity offset"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_master_60B1_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_master_60B2_val = {
	.sub0 = 0,
};

static co_obj_t od_master_60B2 = {
	.node = { .key = &od_master_60B2.idx },
	.idx = 0x60B2,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque offset"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_60B2_val,
	.size = sizeof(od_master_60B2_val)
};

static co_sub_t od_master_60B2sub0 = {
	.node = { .key = &od_master_60B2sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque offset"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_master_60B2_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_master_60FF_val = {
	.sub0 = 0,
};

static co_obj_t od_master_60FF = {
	.node = { .key = &od_master_60FF.idx },
	.idx = 0x60FF,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target velocity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_60FF_val,
	.size = sizeof(od_master_60FF_val)
};

static co_sub_t od_master_60FFsub0 = {
	.node = { .key = &od_master_60FFsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target velocity"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_master_60FF_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_master_6502_val = {
	.sub0 = 0x000003CD,
};

static co_obj_t od_master_6502 = {
	.node = { .key = &od_master_6502.idx },
	.idx = 0x6502,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Supported drive modes"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_6502_val,
	.size = sizeof(od_master_6502_val)
};

static co_sub_t od_master_6502sub0 = {
	.node = { .key = &od_master_6502sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Supported drive modes"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x000003CD },
#endif
	.val = &od_master_6502_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

// suppress missing-prototype warning
co_dev_t * od_master_init(void);
co_dev_t *
od_master_init(void) {
	static co_dev_t *dev = NULL;
	if (!dev) {
		dev = &od_master;

		co_dev_insert_obj(&od_master, &od_master_1000);
		co_obj_insert_sub(&od_master_1000, &od_master_1000sub0);

		co_dev_insert_obj(&od_master, &od_master_1001);
		co_obj_insert_sub(&od_master_1001, &od_master_1001sub0);

		co_dev_insert_obj(&od_master, &od_master_1005);
		co_obj_insert_sub(&od_master_1005, &od_master_1005sub0);

		co_dev_insert_obj(&od_master, &od_master_1006);
		co_obj_insert_sub(&od_master_1006, &od_master_1006sub0);

		co_dev_insert_obj(&od_master, &od_master_1016);
		co_obj_insert_sub(&od_master_1016, &od_master_1016sub0);
		co_obj_insert_sub(&od_master_1016, &od_master_1016sub1);

		co_dev_insert_obj(&od_master, &od_master_1017);
		co_obj_insert_sub(&od_master_1017, &od_master_1017sub0);

		co_dev_insert_obj(&od_master, &od_master_1018);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub0);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub1);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub2);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub3);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub4);

		co_dev_insert_obj(&od_master, &od_master_1019);
		co_obj_insert_sub(&od_master_1019, &od_master_1019sub0);

		co_dev_insert_obj(&od_master, &od_master_1280);
		co_obj_insert_sub(&od_master_1280, &od_master_1280sub0);
		co_obj_insert_sub(&od_master_1280, &od_master_1280sub1);
		co_obj_insert_sub(&od_master_1280, &od_master_1280sub2);
		co_obj_insert_sub(&od_master_1280, &od_master_1280sub3);

		co_dev_insert_obj(&od_master, &od_master_1400);
		co_obj_insert_sub(&od_master_1400, &od_master_1400sub0);
		co_obj_insert_sub(&od_master_1400, &od_master_1400sub1);
		co_obj_insert_sub(&od_master_1400, &od_master_1400sub2);

		co_dev_insert_obj(&od_master, &od_master_1600);
		co_obj_insert_sub(&od_master_1600, &od_master_1600sub0);
		co_obj_insert_sub(&od_master_1600, &od_master_1600sub1);
		co_obj_insert_sub(&od_master_1600, &od_master_1600sub2);

		co_dev_insert_obj(&od_master, &od_master_1800);
		co_obj_insert_sub(&od_master_1800, &od_master_1800sub0);
		co_obj_insert_sub(&od_master_1800, &od_master_1800sub1);
		co_obj_insert_sub(&od_master_1800, &od_master_1800sub2);

		co_dev_insert_obj(&od_master, &od_master_1A00);
		co_obj_insert_sub(&od_master_1A00, &od_master_1A00sub0);
		co_obj_insert_sub(&od_master_1A00, &od_master_1A00sub1);
		co_obj_insert_sub(&od_master_1A00, &od_master_1A00sub2);

		co_dev_insert_obj(&od_master, &od_master_1F80);
		co_obj_insert_sub(&od_master_1F80, &od_master_1F80sub0);

		co_dev_insert_obj(&od_master, &od_master_1F81);
		co_obj_insert_sub(&od_master_1F81, &od_master_1F81sub0);
		co_obj_insert_sub(&od_master_1F81, &od_master_1F81sub1);
		co_obj_insert_sub(&od_master_1F81, &od_master_1F81sub2);

		co_dev_insert_obj(&od_master, &od_master_2000);
		co_obj_insert_sub(&od_master_2000, &od_master_2000sub0);
		co_obj_insert_sub(&od_master_2000, &od_master_2000sub1);

		co_dev_insert_obj(&od_master, &od_master_6040);
		co_obj_insert_sub(&od_master_6040, &od_master_6040sub0);

		co_dev_insert_obj(&od_master, &od_master_6041);
		co_obj_insert_sub(&od_master_6041, &od_master_6041sub0);

		co_dev_insert_obj(&od_master, &od_master_6060);
		co_obj_insert_sub(&od_master_6060, &od_master_6060sub0);

		co_dev_insert_obj(&od_master, &od_master_6061);
		co_obj_insert_sub(&od_master_6061, &od_master_6061sub0);

		co_dev_insert_obj(&od_master, &od_master_6064);
		co_obj_insert_sub(&od_master_6064, &od_master_6064sub0);

		co_dev_insert_obj(&od_master, &od_master_6065);
		co_obj_insert_sub(&od_master_6065, &od_master_6065sub0);

		co_dev_insert_obj(&od_master, &od_master_6071);
		co_obj_insert_sub(&od_master_6071, &od_master_6071sub0);

		co_dev_insert_obj(&od_master, &od_master_6072);
		co_obj_insert_sub(&od_master_6072, &od_master_6072sub0);

		co_dev_insert_obj(&od_master, &od_master_6077);
		co_obj_insert_sub(&od_master_6077, &od_master_6077sub0);

		co_dev_insert_obj(&od_master, &od_master_607A);
		co_obj_insert_sub(&od_master_607A, &od_master_607Asub0);

		co_dev_insert_obj(&od_master, &od_master_6081);
		co_obj_insert_sub(&od_master_6081, &od_master_6081sub0);

		co_dev_insert_obj(&od_master, &od_master_60B1);
		co_obj_insert_sub(&od_master_60B1, &od_master_60B1sub0);

		co_dev_insert_obj(&od_master, &od_master_60B2);
		co_obj_insert_sub(&od_master_60B2, &od_master_60B2sub0);

		co_dev_insert_obj(&od_master, &od_master_60FF);
		co_obj_insert_sub(&od_master_60FF, &od_master_60FFsub0);

		co_dev_insert_obj(&od_master, &od_master_6502);
		co_obj_insert_sub(&od_master_6502, &od_master_6502sub0);
	}
	return dev;
}
