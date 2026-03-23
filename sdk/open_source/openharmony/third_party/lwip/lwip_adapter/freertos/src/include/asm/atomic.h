
#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "soc_osal.h"

#ifndef atomic_t
#define atomic_t osal_atomic
#endif

#ifndef atomic_inc
#define atomic_inc(atom)     osal_atomic_inc(atom)
#endif

#ifndef atomic_dec
#define atomic_dec(atom)     osal_atomic_dec(atom)
#endif

#ifndef atomic_sub
#define atomic_sub(n, v)     osal_atomic_sub(v, n)
#endif

#ifndef atomic_add
#define atomic_add(n, v)     osal_atomic_add(v, n)
#endif

#ifndef atomic_read
#define atomic_read(atom)    osal_atomic_read(atom)
#endif

#ifndef atomic_set
#define atomic_set(p, v)      osal_atomic_set(p, v)
#endif

#ifndef ATOMIC_INIT
#define ATOMIC_INIT(x)     (x)
#endif

#ifndef atomic_add_return
#define atomic_add_return(i, v)    osal_atomic_add_return(v, i)
#endif

#ifndef atomic_inc_return
#define atomic_inc_return(v)      osal_atomic_inc_return(v)
#endif

#ifndef atomic_dec_return
#define atomic_dec_return(v)      osal_atomic_dec_return(v)
#endif

#ifndef atomic_dec_and_test
#define atomic_dec_and_test(v)    osal_atomic_dec_and_test(v)
#endif

#endif
