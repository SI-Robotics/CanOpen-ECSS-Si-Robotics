#ifndef OD_SLAVE_H_GENERATED_
#define OD_SLAVE_H_GENERATED_

#if !LELY_NO_MALLOC
#error Static object dictionaries are only supported when dynamic memory allocation is disabled.
#endif

#include <lely/co/co.h>

#ifdef __cplusplus
extern "C" {
#endif

co_dev_t * od_slave_init(void);

#ifdef __cplusplus
}
#endif

#endif // OD_SLAVE_H_GENERATED_
