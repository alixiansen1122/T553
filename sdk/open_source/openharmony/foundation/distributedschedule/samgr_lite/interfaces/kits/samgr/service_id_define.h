/*
 * Copyright (c) 2022 CompanyNameMagicTag.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SVR_SERVICE_ID_DEFINE_H
#define SVR_SERVICE_ID_DEFINE_H
#include "cores_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SRV_MASK            (0x7FF)
#define FULL_SRV_MASK       (0xbFF)
#define TRAN_MASK           (0x01)
#define DIR_MASK            (0x01)
#define CORE_NUM_MASK       (0x07)
#define CORE_NUM_BIT_POS    (0x0B)
#define TRAN_ID_BIT_POS     (0x0F)
#define DIR_BIT_POS         (0x0E)

#ifndef PLATFORM_CORE
#define PLATFORM_CORE (CORES_APPLICATION_CORE)
#endif

#define SVR_SERVICE_ID_DEF(srvId, coreNum)  \
    ((((coreNum) & CORE_NUM_MASK) << CORE_NUM_BIT_POS) | ((srvId) & SRV_MASK))

#define SET_SERVICE_MSG_DIR(srvIdFull, dir) ((dir & DIR_MASK) << DIR_BIT_POS | (srvIdFull) & FULL_SRV_MASK)

#define COREID_MASK (0xF000)

#define GET_LOCAL_SERVICE_ID(sid) ((sid) & 0x7FF)

/* service id  bit 15,14,13,12 indicates CPU number */
typedef enum {
    /* CORES_SECURITY_CORE */
    /* CORES_PROTOCOL_CORE */
    /* CORES_APPLICATION_CORE */
    /* CORES_IOMCU_CORE */
    /* CORES_DSP_CORE */
    /* CORES_CGRA_CORE */
    /* CORES_MAX_NUMBER_PHYSICAL */
    BOOTSTRAP_SVR_ID = 0,
    COMMU_SVR_ID,
    BROADCAST_SVR_ID,
    DEMO_SVR_ID,
    ABILITY_SVR_ID,
    BUNDLE_MGR_SVR_ID,
    UI_SVR_ID,
    HEALTH_SVR_ID,
    SENSOR_ALG_ACCEL_SVR_ID,
    AUDIO_ALG_ACCEL_SVR_ID,
    MAIN_SVR_ID,
    SENSOR_SVR_ID,
    POWER_SVR_ID,
    BATTERY_SVR_ID,
    DISPLAY_SVR_ID,
#ifdef CONFIG_CGRA_TEST
    TEST_SVR_ID,
#endif
    RTC_SVR_ID,
    STORAGE_SVR_ID,
    MAX_SVR_ID,
    WEARENGINE_ID,
} ServiceId;

typedef enum {
    REMOTE_TRAN,
    LOCAL_TRAN
} TranId;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif // SVR_SERVICE_ID_DEFINE_H

