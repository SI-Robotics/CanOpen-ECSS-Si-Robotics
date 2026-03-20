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
	.vendor_name = CO_DEV_STRING("ZeroErr eRob"),
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
} od_slave_1400_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000181,
	.sub2 = 0x01,
};

static co_obj_t od_slave_1400 = {
	.node = { .key = &od_slave_1400.idx },
	.idx = 0x1400,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("RPDO1 communication parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1400_val,
	.size = sizeof(od_slave_1400_val)
};

static co_sub_t od_slave_1400sub0 = {
	.node = { .key = &od_slave_1400sub0.subidx },
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
	.val = &od_slave_1400_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1400sub1 = {
	.node = { .key = &od_slave_1400sub1.subidx },
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
	.def = { .u32 = 0x00000181 },
#endif
	.val = &od_slave_1400_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1400sub2 = {
	.node = { .key = &od_slave_1400sub2.subidx },
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
	.val = &od_slave_1400_val.sub2,
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
} od_slave_1600_val = {
	.sub0 = 0x02,
	.sub1 = 0x60400010,
	.sub2 = 0x607A0020,
};

static co_obj_t od_slave_1600 = {
	.node = { .key = &od_slave_1600.idx },
	.idx = 0x1600,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("RPDO1 mapping parameter"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1600_val,
	.size = sizeof(od_slave_1600_val)
};

static co_sub_t od_slave_1600sub0 = {
	.node = { .key = &od_slave_1600sub0.subidx },
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
	.val = &od_slave_1600_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1600sub1 = {
	.node = { .key = &od_slave_1600sub1.subidx },
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
	.val = &od_slave_1600_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1600sub2 = {
	.node = { .key = &od_slave_1600sub2.subidx },
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
	.val = &od_slave_1600_val.sub2,
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
	.sub2 = 0x01,
};

static co_obj_t od_slave_1800 = {
	.node = { .key = &od_slave_1800.idx },
	.idx = 0x1800,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO1 communication parameter"),
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
	.def = { .u8 = 0x01 },
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
	.sub1 = 0x60410010,
	.sub2 = 0x60640020,
};

static co_obj_t od_slave_1A00 = {
	.node = { .key = &od_slave_1A00.idx },
	.idx = 0x1A00,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("TPDO1 mapping parameter"),
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
	.name = CO_DEV_STRING("TPDO1 mapping parameter1"),
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
	.name = CO_DEV_STRING("TPDO1 mapping parameter2"),
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
	co_integer32_t sub0;
} od_slave_20A0_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_20A0 = {
	.node = { .key = &od_slave_20A0.idx },
	.idx = 0x20A0,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Load-side encoder position"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_20A0_val,
	.size = sizeof(od_slave_20A0_val)
};

static co_sub_t od_slave_20A0sub0 = {
	.node = { .key = &od_slave_20A0sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Load-side encoder position"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_slave_20A0_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_slave_2240_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_2240 = {
	.node = { .key = &od_slave_2240.idx },
	.idx = 0x2240,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor encoder position"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_2240_val,
	.size = sizeof(od_slave_2240_val)
};

static co_sub_t od_slave_2240sub0 = {
	.node = { .key = &od_slave_2240sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor encoder position"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_slave_2240_val.sub0,
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
} od_slave_3B68_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_3B68 = {
	.node = { .key = &od_slave_3B68.idx },
	.idx = 0x3B68,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Warning code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_3B68_val,
	.size = sizeof(od_slave_3B68_val)
};

static co_sub_t od_slave_3B68sub0 = {
	.node = { .key = &od_slave_3B68sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Warning code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_3B68_val.sub0,
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
} od_slave_4602_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_4602 = {
	.node = { .key = &od_slave_4602.idx },
	.idx = 0x4602,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Release brake"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_4602_val,
	.size = sizeof(od_slave_4602_val)
};

static co_sub_t od_slave_4602sub0 = {
	.node = { .key = &od_slave_4602sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Release brake"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_4602_val.sub0,
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
} od_slave_603F_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_slave_603F = {
	.node = { .key = &od_slave_603F.idx },
	.idx = 0x603F,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Error code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_603F_val,
	.size = sizeof(od_slave_603F_val)
};

static co_sub_t od_slave_603Fsub0 = {
	.node = { .key = &od_slave_603Fsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Error code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x0000 },
#endif
	.val = &od_slave_603F_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned16_t sub0;
} od_slave_6040_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_slave_6040 = {
	.node = { .key = &od_slave_6040.idx },
	.idx = 0x6040,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Controlword"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6040_val,
	.size = sizeof(od_slave_6040_val)
};

static co_sub_t od_slave_6040sub0 = {
	.node = { .key = &od_slave_6040sub0.subidx },
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
	.val = &od_slave_6040_val.sub0,
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
} od_slave_6041_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_slave_6041 = {
	.node = { .key = &od_slave_6041.idx },
	.idx = 0x6041,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Statusword"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6041_val,
	.size = sizeof(od_slave_6041_val)
};

static co_sub_t od_slave_6041sub0 = {
	.node = { .key = &od_slave_6041sub0.subidx },
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
	.val = &od_slave_6041_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_slave_605A_val = {
	.sub0 = 2,
};

static co_obj_t od_slave_605A = {
	.node = { .key = &od_slave_605A.idx },
	.idx = 0x605A,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Quick stop option code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_605A_val,
	.size = sizeof(od_slave_605A_val)
};

static co_sub_t od_slave_605Asub0 = {
	.node = { .key = &od_slave_605Asub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Quick stop option code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 2 },
#endif
	.val = &od_slave_605A_val.sub0,
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
} od_slave_605B_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_605B = {
	.node = { .key = &od_slave_605B.idx },
	.idx = 0x605B,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Shutdown option code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_605B_val,
	.size = sizeof(od_slave_605B_val)
};

static co_sub_t od_slave_605Bsub0 = {
	.node = { .key = &od_slave_605Bsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Shutdown option code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_slave_605B_val.sub0,
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
} od_slave_605C_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_605C = {
	.node = { .key = &od_slave_605C.idx },
	.idx = 0x605C,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Disable operation option code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_605C_val,
	.size = sizeof(od_slave_605C_val)
};

static co_sub_t od_slave_605Csub0 = {
	.node = { .key = &od_slave_605Csub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Disable operation option code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_slave_605C_val.sub0,
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
} od_slave_605D_val = {
	.sub0 = 1,
};

static co_obj_t od_slave_605D = {
	.node = { .key = &od_slave_605D.idx },
	.idx = 0x605D,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Halt option code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_605D_val,
	.size = sizeof(od_slave_605D_val)
};

static co_sub_t od_slave_605Dsub0 = {
	.node = { .key = &od_slave_605Dsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Halt option code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 1 },
#endif
	.val = &od_slave_605D_val.sub0,
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
} od_slave_605E_val = {
	.sub0 = 1,
};

static co_obj_t od_slave_605E = {
	.node = { .key = &od_slave_605E.idx },
	.idx = 0x605E,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Fault reaction option code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_605E_val,
	.size = sizeof(od_slave_605E_val)
};

static co_sub_t od_slave_605Esub0 = {
	.node = { .key = &od_slave_605Esub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Fault reaction option code"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 1 },
#endif
	.val = &od_slave_605E_val.sub0,
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
} od_slave_6060_val = {
	.sub0 = 8,
};

static co_obj_t od_slave_6060 = {
	.node = { .key = &od_slave_6060.idx },
	.idx = 0x6060,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Modes of operation"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6060_val,
	.size = sizeof(od_slave_6060_val)
};

static co_sub_t od_slave_6060sub0 = {
	.node = { .key = &od_slave_6060sub0.subidx },
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
	.val = &od_slave_6060_val.sub0,
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
} od_slave_6061_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6061 = {
	.node = { .key = &od_slave_6061.idx },
	.idx = 0x6061,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Modes of operation display"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6061_val,
	.size = sizeof(od_slave_6061_val)
};

static co_sub_t od_slave_6061sub0 = {
	.node = { .key = &od_slave_6061sub0.subidx },
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
	.val = &od_slave_6061_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_slave_6064_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6064 = {
	.node = { .key = &od_slave_6064.idx },
	.idx = 0x6064,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position actual value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6064_val,
	.size = sizeof(od_slave_6064_val)
};

static co_sub_t od_slave_6064sub0 = {
	.node = { .key = &od_slave_6064sub0.subidx },
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
	.val = &od_slave_6064_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_slave_6065_val = {
	.sub0 = 0x00000001,
};

static co_obj_t od_slave_6065 = {
	.node = { .key = &od_slave_6065.idx },
	.idx = 0x6065,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Following error window"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6065_val,
	.size = sizeof(od_slave_6065_val)
};

static co_sub_t od_slave_6065sub0 = {
	.node = { .key = &od_slave_6065sub0.subidx },
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
	.val = &od_slave_6065_val.sub0,
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
} od_slave_6067_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6067 = {
	.node = { .key = &od_slave_6067.idx },
	.idx = 0x6067,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position window"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6067_val,
	.size = sizeof(od_slave_6067_val)
};

static co_sub_t od_slave_6067sub0 = {
	.node = { .key = &od_slave_6067sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position window"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6067_val.sub0,
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
} od_slave_6068_val = {
	.sub0 = 0x000A,
};

static co_obj_t od_slave_6068 = {
	.node = { .key = &od_slave_6068.idx },
	.idx = 0x6068,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position window time"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6068_val,
	.size = sizeof(od_slave_6068_val)
};

static co_sub_t od_slave_6068sub0 = {
	.node = { .key = &od_slave_6068sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position window time"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x000A },
#endif
	.val = &od_slave_6068_val.sub0,
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
} od_slave_606C_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_606C = {
	.node = { .key = &od_slave_606C.idx },
	.idx = 0x606C,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Actual velocity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_606C_val,
	.size = sizeof(od_slave_606C_val)
};

static co_sub_t od_slave_606Csub0 = {
	.node = { .key = &od_slave_606Csub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Actual velocity"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_slave_606C_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_slave_6071_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6071 = {
	.node = { .key = &od_slave_6071.idx },
	.idx = 0x6071,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target torque"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6071_val,
	.size = sizeof(od_slave_6071_val)
};

static co_sub_t od_slave_6071sub0 = {
	.node = { .key = &od_slave_6071sub0.subidx },
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
	.val = &od_slave_6071_val.sub0,
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
} od_slave_6072_val = {
	.sub0 = 0x0000,
};

static co_obj_t od_slave_6072 = {
	.node = { .key = &od_slave_6072.idx },
	.idx = 0x6072,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max torque"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6072_val,
	.size = sizeof(od_slave_6072_val)
};

static co_sub_t od_slave_6072sub0 = {
	.node = { .key = &od_slave_6072sub0.subidx },
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
	.val = &od_slave_6072_val.sub0,
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
} od_slave_6073_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6073 = {
	.node = { .key = &od_slave_6073.idx },
	.idx = 0x6073,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max current"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6073_val,
	.size = sizeof(od_slave_6073_val)
};

static co_sub_t od_slave_6073sub0 = {
	.node = { .key = &od_slave_6073sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max current"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_slave_6073_val.sub0,
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
} od_slave_6074_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6074 = {
	.node = { .key = &od_slave_6074.idx },
	.idx = 0x6074,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque demand value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6074_val,
	.size = sizeof(od_slave_6074_val)
};

static co_sub_t od_slave_6074sub0 = {
	.node = { .key = &od_slave_6074sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque demand value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_slave_6074_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_slave_6075_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6075 = {
	.node = { .key = &od_slave_6075.idx },
	.idx = 0x6075,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor rated current"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6075_val,
	.size = sizeof(od_slave_6075_val)
};

static co_sub_t od_slave_6075sub0 = {
	.node = { .key = &od_slave_6075sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor rated current"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6075_val.sub0,
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
} od_slave_6076_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6076 = {
	.node = { .key = &od_slave_6076.idx },
	.idx = 0x6076,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor rated torque"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6076_val,
	.size = sizeof(od_slave_6076_val)
};

static co_sub_t od_slave_6076sub0 = {
	.node = { .key = &od_slave_6076sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor rated torque"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6076_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_slave_6077_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6077 = {
	.node = { .key = &od_slave_6077.idx },
	.idx = 0x6077,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque actual value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6077_val,
	.size = sizeof(od_slave_6077_val)
};

static co_sub_t od_slave_6077sub0 = {
	.node = { .key = &od_slave_6077sub0.subidx },
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
	.val = &od_slave_6077_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_slave_6078_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_6078 = {
	.node = { .key = &od_slave_6078.idx },
	.idx = 0x6078,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor actual current"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6078_val,
	.size = sizeof(od_slave_6078_val)
};

static co_sub_t od_slave_6078sub0 = {
	.node = { .key = &od_slave_6078sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Motor actual current"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i16 = CO_INTEGER16_MIN },
	.max = { .i16 = CO_INTEGER16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i16 = 0 },
#endif
	.val = &od_slave_6078_val.sub0,
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
} od_slave_6079_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6079 = {
	.node = { .key = &od_slave_6079.idx },
	.idx = 0x6079,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("DC link circuit voltage"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6079_val,
	.size = sizeof(od_slave_6079_val)
};

static co_sub_t od_slave_6079sub0 = {
	.node = { .key = &od_slave_6079sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("DC link circuit voltage"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6079_val.sub0,
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
} od_slave_607A_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_607A = {
	.node = { .key = &od_slave_607A.idx },
	.idx = 0x607A,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target position"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_607A_val,
	.size = sizeof(od_slave_607A_val)
};

static co_sub_t od_slave_607Asub0 = {
	.node = { .key = &od_slave_607Asub0.subidx },
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
	.val = &od_slave_607A_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer32_t sub0;
} od_slave_607C_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_607C = {
	.node = { .key = &od_slave_607C.idx },
	.idx = 0x607C,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Home offset"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_607C_val,
	.size = sizeof(od_slave_607C_val)
};

static co_sub_t od_slave_607Csub0 = {
	.node = { .key = &od_slave_607Csub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Home offset"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_slave_607C_val.sub0,
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
	co_integer32_t sub1;
	co_integer32_t sub2;
} od_slave_607D_val = {
	.sub0 = 0x02,
	.sub1 = -2147483647,
	.sub2 = CO_INTEGER32_MAX,
};

static co_obj_t od_slave_607D = {
	.node = { .key = &od_slave_607D.idx },
	.idx = 0x607D,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Software position limit"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_607D_val,
	.size = sizeof(od_slave_607D_val)
};

static co_sub_t od_slave_607Dsub0 = {
	.node = { .key = &od_slave_607Dsub0.subidx },
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
	.val = &od_slave_607D_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_607Dsub1 = {
	.node = { .key = &od_slave_607Dsub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Min position limit"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = -2147483647 },
#endif
	.val = &od_slave_607D_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_607Dsub2 = {
	.node = { .key = &od_slave_607Dsub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max position limit"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = CO_INTEGER32_MAX },
#endif
	.val = &od_slave_607D_val.sub2,
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
} od_slave_607E_val = {
	.sub0 = 0x00,
};

static co_obj_t od_slave_607E = {
	.node = { .key = &od_slave_607E.idx },
	.idx = 0x607E,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Polarity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_607E_val,
	.size = sizeof(od_slave_607E_val)
};

static co_sub_t od_slave_607Esub0 = {
	.node = { .key = &od_slave_607Esub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Polarity"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_slave_607E_val.sub0,
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
} od_slave_607F_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_607F = {
	.node = { .key = &od_slave_607F.idx },
	.idx = 0x607F,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max profile velocity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_607F_val,
	.size = sizeof(od_slave_607F_val)
};

static co_sub_t od_slave_607Fsub0 = {
	.node = { .key = &od_slave_607Fsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max profile velocity"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_607F_val.sub0,
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
} od_slave_6080_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6080 = {
	.node = { .key = &od_slave_6080.idx },
	.idx = 0x6080,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max motor speed"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6080_val,
	.size = sizeof(od_slave_6080_val)
};

static co_sub_t od_slave_6080sub0 = {
	.node = { .key = &od_slave_6080sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max motor speed"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6080_val.sub0,
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
} od_slave_6081_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6081 = {
	.node = { .key = &od_slave_6081.idx },
	.idx = 0x6081,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile velocity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6081_val,
	.size = sizeof(od_slave_6081_val)
};

static co_sub_t od_slave_6081sub0 = {
	.node = { .key = &od_slave_6081sub0.subidx },
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
	.val = &od_slave_6081_val.sub0,
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
} od_slave_6083_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6083 = {
	.node = { .key = &od_slave_6083.idx },
	.idx = 0x6083,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile acceleration"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6083_val,
	.size = sizeof(od_slave_6083_val)
};

static co_sub_t od_slave_6083sub0 = {
	.node = { .key = &od_slave_6083sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile acceleration"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6083_val.sub0,
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
} od_slave_6084_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6084 = {
	.node = { .key = &od_slave_6084.idx },
	.idx = 0x6084,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile deceleration"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6084_val,
	.size = sizeof(od_slave_6084_val)
};

static co_sub_t od_slave_6084sub0 = {
	.node = { .key = &od_slave_6084sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Profile deceleration"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6084_val.sub0,
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
} od_slave_6085_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6085 = {
	.node = { .key = &od_slave_6085.idx },
	.idx = 0x6085,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Quick stop deceleration"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6085_val,
	.size = sizeof(od_slave_6085_val)
};

static co_sub_t od_slave_6085sub0 = {
	.node = { .key = &od_slave_6085sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Quick stop deceleration"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6085_val.sub0,
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
} od_slave_6087_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_6087 = {
	.node = { .key = &od_slave_6087.idx },
	.idx = 0x6087,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque slope"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6087_val,
	.size = sizeof(od_slave_6087_val)
};

static co_sub_t od_slave_6087sub0 = {
	.node = { .key = &od_slave_6087sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque slope"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_6087_val.sub0,
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
} od_slave_60B0_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_60B0 = {
	.node = { .key = &od_slave_60B0.idx },
	.idx = 0x60B0,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position offset"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60B0_val,
	.size = sizeof(od_slave_60B0_val)
};

static co_sub_t od_slave_60B0sub0 = {
	.node = { .key = &od_slave_60B0sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Position offset"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_slave_60B0_val.sub0,
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
} od_slave_60B1_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_60B1 = {
	.node = { .key = &od_slave_60B1.idx },
	.idx = 0x60B1,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Velocity offset"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60B1_val,
	.size = sizeof(od_slave_60B1_val)
};

static co_sub_t od_slave_60B1sub0 = {
	.node = { .key = &od_slave_60B1sub0.subidx },
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
	.val = &od_slave_60B1_val.sub0,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_integer16_t sub0;
} od_slave_60B2_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_60B2 = {
	.node = { .key = &od_slave_60B2.idx },
	.idx = 0x60B2,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Torque offset"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60B2_val,
	.size = sizeof(od_slave_60B2_val)
};

static co_sub_t od_slave_60B2sub0 = {
	.node = { .key = &od_slave_60B2sub0.subidx },
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
	.val = &od_slave_60B2_val.sub0,
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
} od_slave_60C5_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_60C5 = {
	.node = { .key = &od_slave_60C5.idx },
	.idx = 0x60C5,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max acceleration"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60C5_val,
	.size = sizeof(od_slave_60C5_val)
};

static co_sub_t od_slave_60C5sub0 = {
	.node = { .key = &od_slave_60C5sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max acceleration"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_60C5_val.sub0,
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
} od_slave_60C6_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_60C6 = {
	.node = { .key = &od_slave_60C6.idx },
	.idx = 0x60C6,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max deceleration"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60C6_val,
	.size = sizeof(od_slave_60C6_val)
};

static co_sub_t od_slave_60C6sub0 = {
	.node = { .key = &od_slave_60C6sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Max deceleration"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_60C6_val.sub0,
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
} od_slave_60E0_val = {
	.sub0 = 0x03E8,
};

static co_obj_t od_slave_60E0 = {
	.node = { .key = &od_slave_60E0.idx },
	.idx = 0x60E0,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Positive torque limit value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60E0_val,
	.size = sizeof(od_slave_60E0_val)
};

static co_sub_t od_slave_60E0sub0 = {
	.node = { .key = &od_slave_60E0sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Positive torque limit value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x03E8 },
#endif
	.val = &od_slave_60E0_val.sub0,
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
} od_slave_60E1_val = {
	.sub0 = 0x03E8,
};

static co_obj_t od_slave_60E1 = {
	.node = { .key = &od_slave_60E1.idx },
	.idx = 0x60E1,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Negative torque limit value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60E1_val,
	.size = sizeof(od_slave_60E1_val)
};

static co_sub_t od_slave_60E1sub0 = {
	.node = { .key = &od_slave_60E1sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Negative torque limit value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x03E8 },
#endif
	.val = &od_slave_60E1_val.sub0,
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
} od_slave_60F4_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_60F4 = {
	.node = { .key = &od_slave_60F4.idx },
	.idx = 0x60F4,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Following error actual value"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60F4_val,
	.size = sizeof(od_slave_60F4_val)
};

static co_sub_t od_slave_60F4sub0 = {
	.node = { .key = &od_slave_60F4sub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_INTEGER32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Following error actual value"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .i32 = CO_INTEGER32_MIN },
	.max = { .i32 = CO_INTEGER32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .i32 = 0 },
#endif
	.val = &od_slave_60F4_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_slave_60FD_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_60FD = {
	.node = { .key = &od_slave_60FD.idx },
	.idx = 0x60FD,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Digital inputs"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60FD_val,
	.size = sizeof(od_slave_60FD_val)
};

static co_sub_t od_slave_60FDsub0 = {
	.node = { .key = &od_slave_60FDsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Digital inputs"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_60FD_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 1,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static struct {
	co_unsigned32_t sub0;
} od_slave_60FE_val = {
	.sub0 = 0x00000000,
};

static co_obj_t od_slave_60FE = {
	.node = { .key = &od_slave_60FE.idx },
	.idx = 0x60FE,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Digital outputs"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60FE_val,
	.size = sizeof(od_slave_60FE_val)
};

static co_sub_t od_slave_60FEsub0 = {
	.node = { .key = &od_slave_60FEsub0.subidx },
	.subidx = 0x00,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Digital outputs"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_60FE_val.sub0,
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
} od_slave_60FF_val = {
	.sub0 = 0,
};

static co_obj_t od_slave_60FF = {
	.node = { .key = &od_slave_60FF.idx },
	.idx = 0x60FF,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Target velocity"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_60FF_val,
	.size = sizeof(od_slave_60FF_val)
};

static co_sub_t od_slave_60FFsub0 = {
	.node = { .key = &od_slave_60FFsub0.subidx },
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
	.val = &od_slave_60FF_val.sub0,
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
} od_slave_6502_val = {
	.sub0 = 0x000003CD,
};

static co_obj_t od_slave_6502 = {
	.node = { .key = &od_slave_6502.idx },
	.idx = 0x6502,
	.code = CO_OBJECT_VAR,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Supported drive modes"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_6502_val,
	.size = sizeof(od_slave_6502_val)
};

static co_sub_t od_slave_6502sub0 = {
	.node = { .key = &od_slave_6502sub0.subidx },
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
	.val = &od_slave_6502_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
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

		co_dev_insert_obj(&od_slave, &od_slave_1400);
		co_obj_insert_sub(&od_slave_1400, &od_slave_1400sub0);
		co_obj_insert_sub(&od_slave_1400, &od_slave_1400sub1);
		co_obj_insert_sub(&od_slave_1400, &od_slave_1400sub2);

		co_dev_insert_obj(&od_slave, &od_slave_1600);
		co_obj_insert_sub(&od_slave_1600, &od_slave_1600sub0);
		co_obj_insert_sub(&od_slave_1600, &od_slave_1600sub1);
		co_obj_insert_sub(&od_slave_1600, &od_slave_1600sub2);

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

		co_dev_insert_obj(&od_slave, &od_slave_20A0);
		co_obj_insert_sub(&od_slave_20A0, &od_slave_20A0sub0);

		co_dev_insert_obj(&od_slave, &od_slave_2240);
		co_obj_insert_sub(&od_slave_2240, &od_slave_2240sub0);

		co_dev_insert_obj(&od_slave, &od_slave_3B68);
		co_obj_insert_sub(&od_slave_3B68, &od_slave_3B68sub0);

		co_dev_insert_obj(&od_slave, &od_slave_4602);
		co_obj_insert_sub(&od_slave_4602, &od_slave_4602sub0);

		co_dev_insert_obj(&od_slave, &od_slave_603F);
		co_obj_insert_sub(&od_slave_603F, &od_slave_603Fsub0);

		co_dev_insert_obj(&od_slave, &od_slave_6040);
		co_obj_insert_sub(&od_slave_6040, &od_slave_6040sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6041);
		co_obj_insert_sub(&od_slave_6041, &od_slave_6041sub0);

		co_dev_insert_obj(&od_slave, &od_slave_605A);
		co_obj_insert_sub(&od_slave_605A, &od_slave_605Asub0);

		co_dev_insert_obj(&od_slave, &od_slave_605B);
		co_obj_insert_sub(&od_slave_605B, &od_slave_605Bsub0);

		co_dev_insert_obj(&od_slave, &od_slave_605C);
		co_obj_insert_sub(&od_slave_605C, &od_slave_605Csub0);

		co_dev_insert_obj(&od_slave, &od_slave_605D);
		co_obj_insert_sub(&od_slave_605D, &od_slave_605Dsub0);

		co_dev_insert_obj(&od_slave, &od_slave_605E);
		co_obj_insert_sub(&od_slave_605E, &od_slave_605Esub0);

		co_dev_insert_obj(&od_slave, &od_slave_6060);
		co_obj_insert_sub(&od_slave_6060, &od_slave_6060sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6061);
		co_obj_insert_sub(&od_slave_6061, &od_slave_6061sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6064);
		co_obj_insert_sub(&od_slave_6064, &od_slave_6064sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6065);
		co_obj_insert_sub(&od_slave_6065, &od_slave_6065sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6067);
		co_obj_insert_sub(&od_slave_6067, &od_slave_6067sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6068);
		co_obj_insert_sub(&od_slave_6068, &od_slave_6068sub0);

		co_dev_insert_obj(&od_slave, &od_slave_606C);
		co_obj_insert_sub(&od_slave_606C, &od_slave_606Csub0);

		co_dev_insert_obj(&od_slave, &od_slave_6071);
		co_obj_insert_sub(&od_slave_6071, &od_slave_6071sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6072);
		co_obj_insert_sub(&od_slave_6072, &od_slave_6072sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6073);
		co_obj_insert_sub(&od_slave_6073, &od_slave_6073sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6074);
		co_obj_insert_sub(&od_slave_6074, &od_slave_6074sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6075);
		co_obj_insert_sub(&od_slave_6075, &od_slave_6075sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6076);
		co_obj_insert_sub(&od_slave_6076, &od_slave_6076sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6077);
		co_obj_insert_sub(&od_slave_6077, &od_slave_6077sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6078);
		co_obj_insert_sub(&od_slave_6078, &od_slave_6078sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6079);
		co_obj_insert_sub(&od_slave_6079, &od_slave_6079sub0);

		co_dev_insert_obj(&od_slave, &od_slave_607A);
		co_obj_insert_sub(&od_slave_607A, &od_slave_607Asub0);

		co_dev_insert_obj(&od_slave, &od_slave_607C);
		co_obj_insert_sub(&od_slave_607C, &od_slave_607Csub0);

		co_dev_insert_obj(&od_slave, &od_slave_607D);
		co_obj_insert_sub(&od_slave_607D, &od_slave_607Dsub0);
		co_obj_insert_sub(&od_slave_607D, &od_slave_607Dsub1);
		co_obj_insert_sub(&od_slave_607D, &od_slave_607Dsub2);

		co_dev_insert_obj(&od_slave, &od_slave_607E);
		co_obj_insert_sub(&od_slave_607E, &od_slave_607Esub0);

		co_dev_insert_obj(&od_slave, &od_slave_607F);
		co_obj_insert_sub(&od_slave_607F, &od_slave_607Fsub0);

		co_dev_insert_obj(&od_slave, &od_slave_6080);
		co_obj_insert_sub(&od_slave_6080, &od_slave_6080sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6081);
		co_obj_insert_sub(&od_slave_6081, &od_slave_6081sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6083);
		co_obj_insert_sub(&od_slave_6083, &od_slave_6083sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6084);
		co_obj_insert_sub(&od_slave_6084, &od_slave_6084sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6085);
		co_obj_insert_sub(&od_slave_6085, &od_slave_6085sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6087);
		co_obj_insert_sub(&od_slave_6087, &od_slave_6087sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60B0);
		co_obj_insert_sub(&od_slave_60B0, &od_slave_60B0sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60B1);
		co_obj_insert_sub(&od_slave_60B1, &od_slave_60B1sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60B2);
		co_obj_insert_sub(&od_slave_60B2, &od_slave_60B2sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60C5);
		co_obj_insert_sub(&od_slave_60C5, &od_slave_60C5sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60C6);
		co_obj_insert_sub(&od_slave_60C6, &od_slave_60C6sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60E0);
		co_obj_insert_sub(&od_slave_60E0, &od_slave_60E0sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60E1);
		co_obj_insert_sub(&od_slave_60E1, &od_slave_60E1sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60F4);
		co_obj_insert_sub(&od_slave_60F4, &od_slave_60F4sub0);

		co_dev_insert_obj(&od_slave, &od_slave_60FD);
		co_obj_insert_sub(&od_slave_60FD, &od_slave_60FDsub0);

		co_dev_insert_obj(&od_slave, &od_slave_60FE);
		co_obj_insert_sub(&od_slave_60FE, &od_slave_60FEsub0);

		co_dev_insert_obj(&od_slave, &od_slave_60FF);
		co_obj_insert_sub(&od_slave_60FF, &od_slave_60FFsub0);

		co_dev_insert_obj(&od_slave, &od_slave_6502);
		co_obj_insert_sub(&od_slave_6502, &od_slave_6502sub0);
	}
	return dev;
}
