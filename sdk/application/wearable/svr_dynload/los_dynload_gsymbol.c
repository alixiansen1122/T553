/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: svr dynload implementation.
 * Author:
 * Create: 2024-10-30
 */

#include "los_elf_symbol_pri.h"

#define SYMBOL_ENTRY(_l, _symbol) \
extern void _symbol(void); \
LDSymbol _l LOS_HAL_TABLE_ENTRY(dynload_gsymbol) = \
{ \
    #_symbol, \
    (const INT8 *)_symbol \
};

SYMBOL_ENTRY(printf_symbol, printf)
SYMBOL_ENTRY(strstr_symbol, strstr)