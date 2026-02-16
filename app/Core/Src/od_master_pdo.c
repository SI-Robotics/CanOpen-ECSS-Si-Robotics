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
	.vendor_name = CO_DEV_STRING("Lely Industries N.V."),
#endif
	.vendor_id = 0x00000360,
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
	.lss = 1,
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
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
} od_master_1016_val = {
	.sub0 = 0x01,
	.sub1 = 0x00020100,
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
	.name = CO_DEV_STRING("NrOfObjects"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x01 },
#endif
	.val = &od_master_1016_val.sub0,
	.access = CO_ACCESS_RO,
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
	.name = CO_DEV_STRING("Consumer heartbeat time1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
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
	co_unsigned8_t sub0;
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
	co_unsigned32_t sub3;
	co_unsigned32_t sub4;
} od_master_1018_val = {
	.sub0 = 0x04,
	.sub1 = 0x00000360,
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
	.def = { .u32 = 0x00000360 },
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
	co_unsigned8_t sub1;
	co_unsigned8_t sub2;
} od_master_1F21_val = {
	.sub0 = 0x02,
	.sub1 = 0x00,
	.sub2 = 0x00,
};

static co_obj_t od_master_1F21 = {
	.node = { .key = &od_master_1F21.idx },
	.idx = 0x1F21,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Store format"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F21_val,
	.size = sizeof(od_master_1F21_val)
};

static co_sub_t od_master_1F21sub0 = {
	.node = { .key = &od_master_1F21sub0.subidx },
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
	.val = &od_master_1F21_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F21sub1 = {
	.node = { .key = &od_master_1F21sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Store format1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_master_1F21_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F21sub2 = {
	.node = { .key = &od_master_1F21sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Store format2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_master_1F21_val.sub2,
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
} od_master_1F55_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x12345678,
};

static co_obj_t od_master_1F55 = {
	.node = { .key = &od_master_1F55.idx },
	.idx = 0x1F55,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Expected software identification"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F55_val,
	.size = sizeof(od_master_1F55_val)
};

static co_sub_t od_master_1F55sub0 = {
	.node = { .key = &od_master_1F55sub0.subidx },
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
	.val = &od_master_1F55_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F55sub1 = {
	.node = { .key = &od_master_1F55sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Expected software identification1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F55_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F55sub2 = {
	.node = { .key = &od_master_1F55sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Expected software identification2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F55_val.sub2,
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
	.sub2 = 0x00000065,
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
	co_unsigned32_t sub1;
	co_unsigned32_t sub2;
} od_master_1F84_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x01020304,
};

static co_obj_t od_master_1F84 = {
	.node = { .key = &od_master_1F84.idx },
	.idx = 0x1F84,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Device type identification"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F84_val,
	.size = sizeof(od_master_1F84_val)
};

static co_sub_t od_master_1F84sub0 = {
	.node = { .key = &od_master_1F84sub0.subidx },
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
	.val = &od_master_1F84_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F84sub1 = {
	.node = { .key = &od_master_1F84sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Device type identification1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F84_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F84sub2 = {
	.node = { .key = &od_master_1F84sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Device type identification2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F84_val.sub2,
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
} od_master_1F85_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x00000360,
};

static co_obj_t od_master_1F85 = {
	.node = { .key = &od_master_1F85.idx },
	.idx = 0x1F85,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Vendor identification"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F85_val,
	.size = sizeof(od_master_1F85_val)
};

static co_sub_t od_master_1F85sub0 = {
	.node = { .key = &od_master_1F85sub0.subidx },
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
	.val = &od_master_1F85_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F85sub1 = {
	.node = { .key = &od_master_1F85sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Vendor identification1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F85_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F85sub2 = {
	.node = { .key = &od_master_1F85sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Vendor identification2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F85_val.sub2,
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
} od_master_1F86_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x00000002,
};

static co_obj_t od_master_1F86 = {
	.node = { .key = &od_master_1F86.idx },
	.idx = 0x1F86,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Product code"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F86_val,
	.size = sizeof(od_master_1F86_val)
};

static co_sub_t od_master_1F86sub0 = {
	.node = { .key = &od_master_1F86sub0.subidx },
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
	.val = &od_master_1F86_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F86sub1 = {
	.node = { .key = &od_master_1F86sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Product code1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F86_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F86sub2 = {
	.node = { .key = &od_master_1F86sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Product code2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F86_val.sub2,
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
} od_master_1F87_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x00000003,
};

static co_obj_t od_master_1F87 = {
	.node = { .key = &od_master_1F87.idx },
	.idx = 0x1F87,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Revision number"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F87_val,
	.size = sizeof(od_master_1F87_val)
};

static co_sub_t od_master_1F87sub0 = {
	.node = { .key = &od_master_1F87sub0.subidx },
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
	.val = &od_master_1F87_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F87sub1 = {
	.node = { .key = &od_master_1F87sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Revision number1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F87_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F87sub2 = {
	.node = { .key = &od_master_1F87sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Revision number2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F87_val.sub2,
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
} od_master_1F88_val = {
	.sub0 = 0x02,
	.sub1 = 0x00000000,
	.sub2 = 0x00000004,
};

static co_obj_t od_master_1F88 = {
	.node = { .key = &od_master_1F88.idx },
	.idx = 0x1F88,
	.code = CO_OBJECT_ARRAY,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Serial number"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_master_1F88_val,
	.size = sizeof(od_master_1F88_val)
};

static co_sub_t od_master_1F88sub0 = {
	.node = { .key = &od_master_1F88sub0.subidx },
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
	.val = &od_master_1F88_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F88sub1 = {
	.node = { .key = &od_master_1F88sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Serial number1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F88_val.sub1,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_master_1F88sub2 = {
	.node = { .key = &od_master_1F88sub2.subidx },
	.subidx = 0x02,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Serial number2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_master_1F88_val.sub2,
	.access = CO_ACCESS_RW,
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

		co_dev_insert_obj(&od_master, &od_master_1016);
		co_obj_insert_sub(&od_master_1016, &od_master_1016sub0);
		co_obj_insert_sub(&od_master_1016, &od_master_1016sub1);

		co_dev_insert_obj(&od_master, &od_master_1018);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub0);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub1);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub2);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub3);
		co_obj_insert_sub(&od_master_1018, &od_master_1018sub4);

		co_dev_insert_obj(&od_master, &od_master_1F21);
		co_obj_insert_sub(&od_master_1F21, &od_master_1F21sub0);
		co_obj_insert_sub(&od_master_1F21, &od_master_1F21sub1);
		co_obj_insert_sub(&od_master_1F21, &od_master_1F21sub2);

		co_dev_insert_obj(&od_master, &od_master_1F55);
		co_obj_insert_sub(&od_master_1F55, &od_master_1F55sub0);
		co_obj_insert_sub(&od_master_1F55, &od_master_1F55sub1);
		co_obj_insert_sub(&od_master_1F55, &od_master_1F55sub2);

		co_dev_insert_obj(&od_master, &od_master_1F80);
		co_obj_insert_sub(&od_master_1F80, &od_master_1F80sub0);

		co_dev_insert_obj(&od_master, &od_master_1F81);
		co_obj_insert_sub(&od_master_1F81, &od_master_1F81sub0);
		co_obj_insert_sub(&od_master_1F81, &od_master_1F81sub1);
		co_obj_insert_sub(&od_master_1F81, &od_master_1F81sub2);

		co_dev_insert_obj(&od_master, &od_master_1F84);
		co_obj_insert_sub(&od_master_1F84, &od_master_1F84sub0);
		co_obj_insert_sub(&od_master_1F84, &od_master_1F84sub1);
		co_obj_insert_sub(&od_master_1F84, &od_master_1F84sub2);

		co_dev_insert_obj(&od_master, &od_master_1F85);
		co_obj_insert_sub(&od_master_1F85, &od_master_1F85sub0);
		co_obj_insert_sub(&od_master_1F85, &od_master_1F85sub1);
		co_obj_insert_sub(&od_master_1F85, &od_master_1F85sub2);

		co_dev_insert_obj(&od_master, &od_master_1F86);
		co_obj_insert_sub(&od_master_1F86, &od_master_1F86sub0);
		co_obj_insert_sub(&od_master_1F86, &od_master_1F86sub1);
		co_obj_insert_sub(&od_master_1F86, &od_master_1F86sub2);

		co_dev_insert_obj(&od_master, &od_master_1F87);
		co_obj_insert_sub(&od_master_1F87, &od_master_1F87sub0);
		co_obj_insert_sub(&od_master_1F87, &od_master_1F87sub1);
		co_obj_insert_sub(&od_master_1F87, &od_master_1F87sub2);

		co_dev_insert_obj(&od_master, &od_master_1F88);
		co_obj_insert_sub(&od_master_1F88, &od_master_1F88sub0);
		co_obj_insert_sub(&od_master_1F88, &od_master_1F88sub1);
		co_obj_insert_sub(&od_master_1F88, &od_master_1F88sub2);
	}
	return dev;
}
