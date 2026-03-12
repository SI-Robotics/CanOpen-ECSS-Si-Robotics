#if !LELY_NO_MALLOC
#error Static object dictionaries are only supported when dynamic memory allocation is disabled.
#endif

#include <lely/co/detail/dev.h>
#include <lely/co/detail/obj.h>
#include <lely/util/cmp.h>

#define CO_DEV_STRING(s) s

static co_dev_t od_slave = {
	.netid = 0,
	.id = 2,
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
} od_slave_1000_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_1000 = {
	.node = { .key = &od_slave_1000.idx },
	.idx = 0x1000,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Device type"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1000_val,
	.size = sizeof(od_slave_1000_val)
};

static co_sub_t od_slave_1000sub0 = {
	.node = { .key = &od_slave_1000sub0.subidx },
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
	.val = &od_slave_1000_val.sub0,
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
} od_slave_1001_val = {
	.sub0 = 0x00,
};

static co_obj_t od_slave_1001 = {
	.node = { .key = &od_slave_1001.idx },
	.idx = 0x1001,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Error register"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1001_val,
	.size = sizeof(od_slave_1001_val)
};

static co_sub_t od_slave_1001sub0 = {
	.node = { .key = &od_slave_1001sub0.subidx },
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
	.val = &od_slave_1001_val.sub0,
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
} od_slave_1005_val = {
	.sub0 = 0x40000080,
};

static co_obj_t od_slave_1005 = {
	.node = { .key = &od_slave_1005.idx },
	.idx = 0x1005,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("COB-ID SYNC"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1005_val,
	.size = sizeof(od_slave_1005_val)
};

static co_sub_t od_slave_1005sub0 = {
	.node = { .key = &od_slave_1005sub0.subidx },
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
	.val = &od_slave_1005_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0 | CO_OBJ_FLAGS_PARAMETER_VALUE,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_slave_1006_val = {
	.sub0 = 0x000F4240,
};

static co_obj_t od_slave_1006 = {
	.node = { .key = &od_slave_1006.idx },
	.idx = 0x1006,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Communication cycle period"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1006_val,
	.size = sizeof(od_slave_1006_val)
};

static co_sub_t od_slave_1006sub0 = {
	.node = { .key = &od_slave_1006sub0.subidx },
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
	.val = &od_slave_1006_val.sub0,
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
} od_slave_1016_val = {
	.sub0 = 0x01,
	.sub1 = 0x00010226,
};

static co_obj_t od_slave_1016 = {
	.node = { .key = &od_slave_1016.idx },
	.idx = 0x1016,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Consumer heartbeat time"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1016_val,
	.size = sizeof(od_slave_1016_val)
};

static co_sub_t od_slave_1016sub0 = {
	.node = { .key = &od_slave_1016sub0.subidx },
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
	.val = &od_slave_1016_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1016sub1 = {
	.node = { .key = &od_slave_1016sub1.subidx },
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
	.def = { .u32 = 0x00010226 },
#endif
	.val = &od_slave_1016_val.sub1,
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
} od_slave_1017_val = {
	.sub0 = 0x01F4,
};

static co_obj_t od_slave_1017 = {
	.node = { .key = &od_slave_1017.idx },
	.idx = 0x1017,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Producer heartbeat time"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1017_val,
	.size = sizeof(od_slave_1017_val)
};

static co_sub_t od_slave_1017sub0 = {
	.node = { .key = &od_slave_1017sub0.subidx },
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
	.val = &od_slave_1017_val.sub0,
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
} od_slave_1018_val = {
	.sub0 = 0x04,
	.sub1 = 0x004E3753,
	.sub2 = 0x00000000,
	.sub3 = 0x00000000,
	.sub4 = 0x00000000,
};

static co_obj_t od_slave_1018 = {
	.node = { .key = &od_slave_1018.idx },
	.idx = 0x1018,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Identity object"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1018_val,
	.size = sizeof(od_slave_1018_val)
};

static co_sub_t od_slave_1018sub0 = {
	.node = { .key = &od_slave_1018sub0.subidx },
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
	.val = &od_slave_1018_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1018sub1 = {
	.node = { .key = &od_slave_1018sub1.subidx },
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
	.val = &od_slave_1018_val.sub1,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1018sub2 = {
	.node = { .key = &od_slave_1018sub2.subidx },
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
	.val = &od_slave_1018_val.sub2,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1018sub3 = {
	.node = { .key = &od_slave_1018sub3.subidx },
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
	.val = &od_slave_1018_val.sub3,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1018sub4 = {
	.node = { .key = &od_slave_1018sub4.subidx },
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
	.val = &od_slave_1018_val.sub4,
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
} od_slave_1019_val = {
	.sub0 = 0x10,
};

static co_obj_t od_slave_1019 = {
	.node = { .key = &od_slave_1019.idx },
	.idx = 0x1019,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Synchronous counter overflow value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1019_val,
	.size = sizeof(od_slave_1019_val)
};

static co_sub_t od_slave_1019sub0 = {
	.node = { .key = &od_slave_1019sub0.subidx },
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
	.val = &od_slave_1019_val.sub0,
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
} od_slave_1800_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000182,
	.sub2 = 0x03,
};

static co_obj_t od_slave_1800 = {
	.node = { .key = &od_slave_1800.idx },
	.idx = 0x1800,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO communication parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1800_val,
	.size = sizeof(od_slave_1800_val)
};

static co_sub_t od_slave_1800sub0 = {
	.node = { .key = &od_slave_1800sub0.subidx },
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
	.val = &od_slave_1800_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1800sub1 = {
	.node = { .key = &od_slave_1800sub1.subidx },
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
	.def = { .u32 = 0x00000182 },
#endif
	.val = &od_slave_1800_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0 | CO_OBJ_FLAGS_DEF_NODEID | CO_OBJ_FLAGS_VAL_NODEID,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1800sub2 = {
	.node = { .key = &od_slave_1800sub2.subidx },
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
	.def = { .u8 = 0x03 },
#endif
	.val = &od_slave_1800_val.sub2,
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
} od_slave_1A00_val = {
	.sub0 = 0x02,
	.sub1 = 0x21000020,
	.sub2 = 0x21010008,
};

static co_obj_t od_slave_1A00 = {
	.node = { .key = &od_slave_1A00.idx },
	.idx = 0x1A00,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO mapping parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1A00_val,
	.size = sizeof(od_slave_1A00_val)
};

static co_sub_t od_slave_1A00sub0 = {
	.node = { .key = &od_slave_1A00sub0.subidx },
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
	.val = &od_slave_1A00_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1A00sub1 = {
	.node = { .key = &od_slave_1A00sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO mapping parameter1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_1A00_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1A00sub2 = {
	.node = { .key = &od_slave_1A00sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO mapping parameter2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_1A00_val.sub2,
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
} od_slave_1F80_val = {
	.sub0 = 0x00000004,
};

static co_obj_t od_slave_1F80 = {
	.node = { .key = &od_slave_1F80.idx },
	.idx = 0x1F80,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("NMT startup"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1F80_val,
	.size = sizeof(od_slave_1F80_val)
};

static co_sub_t od_slave_1F80sub0 = {
	.node = { .key = &od_slave_1F80sub0.subidx },
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
	.def = { .u32 = 0x00000004 },
#endif
	.val = &od_slave_1F80_val.sub0,
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
	co_unsigned8_t sub2;
	co_unsigned8_t sub3;
	co_unsigned8_t sub4;
} od_slave_2000_val = {
	.sub0 = 0x04,
	.sub1 = 0x00,
	.sub2 = 0x03,
	.sub3 = 0x05,
	.sub4 = 0x00,
};

static co_obj_t od_slave_2000 = {
	.node = { .key = &od_slave_2000.idx },
	.idx = 0x2000,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Redundancy object"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_2000_val,
	.size = sizeof(od_slave_2000_val)
};

static co_sub_t od_slave_2000sub0 = {
	.node = { .key = &od_slave_2000sub0.subidx },
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
	.val = &od_slave_2000_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_2000sub1 = {
	.node = { .key = &od_slave_2000sub1.subidx },
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
	.val = &od_slave_2000_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_2000sub2 = {
	.node = { .key = &od_slave_2000sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Ttoggle"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x03 },
#endif
	.val = &od_slave_2000_val.sub2,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_2000sub3 = {
	.node = { .key = &od_slave_2000sub3.subidx },
	.subidx = 0x03,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Ntoggle"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x05 },
#endif
	.val = &od_slave_2000_val.sub3,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_2000sub4 = {
	.node = { .key = &od_slave_2000sub4.subidx },
	.subidx = 0x04,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Ctoggle"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_slave_2000_val.sub4,
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
} od_slave_2100_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_2100 = {
	.node = { .key = &od_slave_2100.idx },
	.idx = 0x2100,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TestUnsigned32"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_2100_val,
	.size = sizeof(od_slave_2100_val)
};

static co_sub_t od_slave_2100sub0 = {
	.node = { .key = &od_slave_2100sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TestUnsigned32"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_2100_val.sub0,
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
} od_slave_2101_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_2101 = {
	.node = { .key = &od_slave_2101.idx },
	.idx = 0x2101,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TestInteger8"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_2101_val,
	.size = sizeof(od_slave_2101_val)
};

static co_sub_t od_slave_2101sub0 = {
	.node = { .key = &od_slave_2101sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TestInteger8"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i8 = CO_INTEGER8_MIN },
	.max = { .i8 = CO_INTEGER8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i8 = 0 },
#endif
	.val = &od_slave_2101_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

// suppress missing-prototype warning
co_dev_t * od_slave_init(void);
co_dev_t *
od_slave_init(void) {
	static co_dev_t *dev = NULL;
	if (!dev) {
		dev = &od_slave;

		co_dev_insert_obj(&od_slave, &od_slave_1000);
		co_obj_insert_sub(&od_slave_1000, &od_slave_1000sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1001);
		co_obj_insert_sub(&od_slave_1001, &od_slave_1001sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1005);
		co_obj_insert_sub(&od_slave_1005, &od_slave_1005sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1006);
		co_obj_insert_sub(&od_slave_1006, &od_slave_1006sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1016);
		co_obj_insert_sub(&od_slave_1016, &od_slave_1016sub0);
		co_obj_insert_sub(&od_slave_1016, &od_slave_1016sub1);

		co_dev_insert_obj(&od_slave, &od_slave_1017);
		co_obj_insert_sub(&od_slave_1017, &od_slave_1017sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1018);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub0);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub1);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub2);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub3);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub4);

		co_dev_insert_obj(&od_slave, &od_slave_1019);
		co_obj_insert_sub(&od_slave_1019, &od_slave_1019sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1800);
		co_obj_insert_sub(&od_slave_1800, &od_slave_1800sub0);
		co_obj_insert_sub(&od_slave_1800, &od_slave_1800sub1);
		co_obj_insert_sub(&od_slave_1800, &od_slave_1800sub2);

		co_dev_insert_obj(&od_slave, &od_slave_1A00);
		co_obj_insert_sub(&od_slave_1A00, &od_slave_1A00sub0);
		co_obj_insert_sub(&od_slave_1A00, &od_slave_1A00sub1);
		co_obj_insert_sub(&od_slave_1A00, &od_slave_1A00sub2);

		co_dev_insert_obj(&od_slave, &od_slave_1F80);
		co_obj_insert_sub(&od_slave_1F80, &od_slave_1F80sub0);

		co_dev_insert_obj(&od_slave, &od_slave_2000);
		co_obj_insert_sub(&od_slave_2000, &od_slave_2000sub0);
		co_obj_insert_sub(&od_slave_2000, &od_slave_2000sub1);
		co_obj_insert_sub(&od_slave_2000, &od_slave_2000sub2);
		co_obj_insert_sub(&od_slave_2000, &od_slave_2000sub3);
		co_obj_insert_sub(&od_slave_2000, &od_slave_2000sub4);

		co_dev_insert_obj(&od_slave, &od_slave_2100);
		co_obj_insert_sub(&od_slave_2100, &od_slave_2100sub0);

		co_dev_insert_obj(&od_slave, &od_slave_2101);
		co_obj_insert_sub(&od_slave_2101, &od_slave_2101sub0);
	}
	return dev;
}
