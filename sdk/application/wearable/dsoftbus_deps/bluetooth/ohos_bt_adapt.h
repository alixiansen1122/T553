/*
 * Copyright (c) CompanyNameMagicTag 2024-2024. All rights reserved.
 * Description: ohos bt adapter header file.
 * Author: CompanyName
 * Create:
 */
#ifndef OHOS_BT_ADAPT_H
#define OHOS_BT_ADAPT_H

#include "bts_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

int32_t ble_gatt_get_client_id(void);
bt_uuid_t ble_gatt_get_uuid(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* OHOS_BT_ADAPT_H */

