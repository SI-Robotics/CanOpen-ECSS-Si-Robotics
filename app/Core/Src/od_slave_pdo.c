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
	.vendor_name = CO_DEV_STRING("Lely Industries N.V."),
#endif
	.vendor_id = 0x00000360,
#if !LELY_NO_CO_OBJ_NAME
	.product_name = CO_DEV_STRING(""),
#endif
	.product_code = 0x00000002,
	.revision = 0x00000003,
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
} od_slave_1000_val = {
	.sub0 = 0x01020304,
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
	.def = { .u32 = 0x01020304 },
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
	co_unsigned16_t sub0;
} od_slave_1017_val = {
	.sub0 = 0x0064,
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
	.def = { .u16 = 0x0000 },
#endif
	.val = &od_slave_1017_val.sub0,
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
	co_unsigned32_t sub3;
	co_unsigned32_t sub4;
} od_slave_1018_val = {
	.sub0 = 0x04,
	.sub1 = 0x00000360,
	.sub2 = 0x00000002,
	.sub3 = 0x00000003,
	.sub4 = 0x00000004,
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
	.def = { .u32 = 0x00000360 },
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
	.def = { .u32 = 0x00000002 },
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
	.def = { .u32 = 0x00000003 },
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
	.flags = 0 | CO_OBJ_FLAGS_PARAMETER_VALUE,
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
	.sub1 = 0x00000202,
	.sub2 = CO_UNSIGNED8_MAX,
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
	.name = CO_DEV_STRING("COB-ID used by RPDO1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000202 },
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
	.name = CO_DEV_STRING("Transmission type RPDO1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = CO_UNSIGNED8_MAX },
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
	.name = CO_DEV_STRING("Number of mapped application objects"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_slave_1600_val.sub0,
	.access = CO_ACCESS_RW,
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
	.name = CO_DEV_STRING("Mapped object 1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x60400010 },
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
	.name = CO_DEV_STRING("Mapped object 2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x607A0020 },
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
	co_unsigned16_t sub3;
	co_unsigned8_t sub4;
	co_unsigned16_t sub5;
} od_slave_1800_val = {
	.sub0 = 0x05,
	.sub1 = 0x00000182,
	.sub2 = CO_UNSIGNED8_MAX,
	.sub3 = 0x0000,
	.sub4 = 0x00,
	.sub5 = 0x0064,
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
	.def = { .u8 = 0x05 },
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
	.name = CO_DEV_STRING("COB-ID used by TPDO1"),
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
	.flags = 0,
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
	.name = CO_DEV_STRING("Transmission type TPDO1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = CO_UNSIGNED8_MAX },
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

static co_sub_t od_slave_1800sub3 = {
	.node = { .key = &od_slave_1800sub3.subidx },
	.subidx = 0x03,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Inhibit time TPDO1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x0000 },
#endif
	.val = &od_slave_1800_val.sub3,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1800sub4 = {
	.node = { .key = &od_slave_1800sub4.subidx },
	.subidx = 0x04,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Reserved"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x00 },
#endif
	.val = &od_slave_1800_val.sub4,
	.access = CO_ACCESS_RW,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1800sub5 = {
	.node = { .key = &od_slave_1800sub5.subidx },
	.subidx = 0x05,
	.type = CO_DEFTYPE_UNSIGNED16,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Event timer TPDO1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u16 = CO_UNSIGNED16_MIN },
	.max = { .u16 = CO_UNSIGNED16_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u16 = 0x0064 },
#endif
	.val = &od_slave_1800_val.sub5,
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
	.name = CO_DEV_STRING("Number of mapped application objects"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x02 },
#endif
	.val = &od_slave_1A00_val.sub0,
	.access = CO_ACCESS_RW,
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
	.name = CO_DEV_STRING("Mapped object 1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x60410010 },
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
	.name = CO_DEV_STRING("Mapped object 2"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x60640020 },
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
	co_unsigned8_t sub0;
	co_domain_t sub1;
} od_slave_1F50_val = {
	.sub0 = 0x01,
	.sub1 = CO_ARRAY_C,
};

static co_obj_t od_slave_1F50 = {
	.node = { .key = &od_slave_1F50.idx },
	.idx = 0x1F50,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program data"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1F50_val,
	.size = sizeof(od_slave_1F50_val)
};

static co_sub_t od_slave_1F50sub0 = {
	.node = { .key = &od_slave_1F50sub0.subidx },
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
	.val = &od_slave_1F50_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1F50sub1 = {
	.node = { .key = &od_slave_1F50sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_DOMAIN,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program Number 1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .dom = CO_DOMAIN_MIN },
	.max = { .dom = CO_DOMAIN_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .dom = CO_ARRAY_C },
#endif
	.val = &od_slave_1F50_val.sub1,
	.access = CO_ACCESS_WO,
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
} od_slave_1F51_val = {
	.sub0 = 0x01,
	.sub1 = 0x01,
};

static co_obj_t od_slave_1F51 = {
	.node = { .key = &od_slave_1F51.idx },
	.idx = 0x1F51,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program control"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1F51_val,
	.size = sizeof(od_slave_1F51_val)
};

static co_sub_t od_slave_1F51sub0 = {
	.node = { .key = &od_slave_1F51sub0.subidx },
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
	.val = &od_slave_1F51_val.sub0,
	.access = CO_ACCESS_CONST,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1F51sub1 = {
	.node = { .key = &od_slave_1F51sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED8,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program Number 1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u8 = CO_UNSIGNED8_MIN },
	.max = { .u8 = CO_UNSIGNED8_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u8 = 0x01 },
#endif
	.val = &od_slave_1F51_val.sub1,
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
} od_slave_1F56_val = {
	.sub0 = 0x01,
	.sub1 = 0x00000000,
};

static co_obj_t od_slave_1F56 = {
	.node = { .key = &od_slave_1F56.idx },
	.idx = 0x1F56,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program software identification"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1F56_val,
	.size = sizeof(od_slave_1F56_val)
};

static co_sub_t od_slave_1F56sub0 = {
	.node = { .key = &od_slave_1F56sub0.subidx },
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
	.val = &od_slave_1F56_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1F56sub1 = {
	.node = { .key = &od_slave_1F56sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program Number 1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_1F56_val.sub1,
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
} od_slave_1F57_val = {
	.sub0 = 0x01,
	.sub1 = 0x00000000,
};

static co_obj_t od_slave_1F57 = {
	.node = { .key = &od_slave_1F57.idx },
	.idx = 0x1F57,
	.code = CO_OBJECT_RECORD,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Flash status identification"),
#endif
	.tree = { .cmp = &uint8_cmp },
	.val = &od_slave_1F57_val,
	.size = sizeof(od_slave_1F57_val)
};

static co_sub_t od_slave_1F57sub0 = {
	.node = { .key = &od_slave_1F57sub0.subidx },
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
	.val = &od_slave_1F57_val.sub0,
	.access = CO_ACCESS_RO,
	.pdo_mapping = 0,
	.flags = 0,
	.dn_ind = &co_sub_default_dn_ind,
#if !LELY_NO_CO_OBJ_UPLOAD
	.up_ind = &co_sub_default_up_ind
#endif
};

static co_sub_t od_slave_1F57sub1 = {
	.node = { .key = &od_slave_1F57sub1.subidx },
	.subidx = 0x01,
	.type = CO_DEFTYPE_UNSIGNED32,
#if !LELY_NO_CO_OBJ_NAME
	.name = CO_DEV_STRING("Program Number 1"),
#endif
#if !LELY_NO_CO_OBJ_LIMITS
	.min = { .u32 = CO_UNSIGNED32_MIN },
	.max = { .u32 = CO_UNSIGNED32_MAX },
#endif
#if !LELY_NO_CO_OBJ_DEFAULT
	.def = { .u32 = 0x00000000 },
#endif
	.val = &od_slave_1F57_val.sub1,
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

		co_dev_insert_obj(&od_slave, &od_slave_1017);
		co_obj_insert_sub(&od_slave_1017, &od_slave_1017sub0);

		co_dev_insert_obj(&od_slave, &od_slave_1018);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub0);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub1);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub2);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub3);
		co_obj_insert_sub(&od_slave_1018, &od_slave_1018sub4);

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
		co_obj_insert_sub(&od_slave_1800, &od_slave_1800sub3);
		co_obj_insert_sub(&od_slave_1800, &od_slave_1800sub4);
		co_obj_insert_sub(&od_slave_1800, &od_slave_1800sub5);

		co_dev_insert_obj(&od_slave, &od_slave_1A00);
		co_obj_insert_sub(&od_slave_1A00, &od_slave_1A00sub0);
		co_obj_insert_sub(&od_slave_1A00, &od_slave_1A00sub1);
		co_obj_insert_sub(&od_slave_1A00, &od_slave_1A00sub2);

		co_dev_insert_obj(&od_slave, &od_slave_1F50);
		co_obj_insert_sub(&od_slave_1F50, &od_slave_1F50sub0);
		co_obj_insert_sub(&od_slave_1F50, &od_slave_1F50sub1);

		co_dev_insert_obj(&od_slave, &od_slave_1F51);
		co_obj_insert_sub(&od_slave_1F51, &od_slave_1F51sub0);
		co_obj_insert_sub(&od_slave_1F51, &od_slave_1F51sub1);

		co_dev_insert_obj(&od_slave, &od_slave_1F56);
		co_obj_insert_sub(&od_slave_1F56, &od_slave_1F56sub0);
		co_obj_insert_sub(&od_slave_1F56, &od_slave_1F56sub1);

		co_dev_insert_obj(&od_slave, &od_slave_1F57);
		co_obj_insert_sub(&od_slave_1F57, &od_slave_1F57sub0);
		co_obj_insert_sub(&od_slave_1F57, &od_slave_1F57sub1);

		co_dev_insert_obj(&od_slave, &od_slave_1F80);
		co_obj_insert_sub(&od_slave_1F80, &od_slave_1F80sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6040);
		co_obj_insert_sub(&od_slave_6040, &od_slave_6040sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6041);
		co_obj_insert_sub(&od_slave_6041, &od_slave_6041sub0);

		co_dev_insert_obj(&od_slave, &od_slave_6064);
		co_obj_insert_sub(&od_slave_6064, &od_slave_6064sub0);

		co_dev_insert_obj(&od_slave, &od_slave_607A);
		co_obj_insert_sub(&od_slave_607A, &od_slave_607Asub0);
	}
	return dev;
}
