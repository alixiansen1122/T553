/*
 * Copyright (c) CompanyNameMagicTag. 2025-2025. All rights reserved.
 * Description: sensor common
 * Author: Media Software Group
 * Create: 2025-06-06
 */


#ifndef HDI_SENSOR_COMMON_H
#define HDI_SENSOR_COMMON_H

#include <stdio.h>
#include "sensor_log.h"
#include "securec.h"

/* err number 管理，约定mudule id为30 */
#define SENSOR_ERRNO_MODULE_ID 30 /* 0x1e */

/* start：0x801e0000  end：0x801effff 返回错误信息均为负数 */
#define SENSOR_ERR_NUMBER(type, code) \
    (int32_t)((((((0x80 << 8) | (SENSOR_ERRNO_MODULE_ID)) << 8) | (type)) << 8) | (code))

typedef enum {
    SENSOR_ETYPE_ALG            = 0x00,
    SENSOR_ETYPE_VSENSOR_MGR    = 0x01,
    SENSOR_ETYPE_DATA_SERVICE   = 0x02,
    SENSOR_ETYPE_FRAME          = 0x03,
    SENSOR_ETYPE_ENTRY_PARA     = 0x04,

    SENSOR_ETYPE_MAX
} SensorErrType;

/* all return values of SENSOR Engine, partition by module */
enum {
    SENSOR_ERROR = -1,
    SENSOR_OK = 0,

    /* 算法错误码 SENSOR_ETYPE_ALG */
    SENSOR_ALG_ERRNO = SENSOR_ERR_NUMBER(SENSOR_ETYPE_ALG, 0), /* 0x801e0000 */

    /* 虚拟算法Sensor管理错误码 SENSOR_ETYPE_VSENSOR_MGR */
    SENSOR_ALG_SERVICE_ERRNO = SENSOR_ERR_NUMBER(SENSOR_ETYPE_VSENSOR_MGR, 0), /* 0x801e0100 */
    SENSOR_CHECK_RELY_SENSORS_NULL,                                            /* 0x801e0101 */
    SENSOR_CHECK_RELY_SENSORS_SENSORIL,                                        /* 0x801e0102 */
    SENSOR_CHECK_PROCESS_DATA_NULL,                                            /* 0x801e0103 */
    SENSOR_CHECK_PROCESS_DATA_TS_ERR,                                          /* 0x801e0104 */
    SENSOR_GET_RELY_SENSOR_INF_SENSORIL,                                       /* 0x801e0105 */
    SENSOR_OPEN_RELY_SENSORS_CHECK_NULL,                                       /* 0x801e0106 */
    SENSOR_VSENSOR_NOT_SUPPORT,                                                /* 0x801e01A1 */
    SENSOR_PERSON_INFO_NULL,                                                   /* 0x801e01A2 */
    SENSOR_VSENSOR_TASK_REG_VENSOR_NULL,                                       /* 0x801e01A5 */
    SENSOR_TASK_SENSOR_DATA_UPDATE_NULL,                                      /* 0x801e01A6 */
    SENSOR_TASK_CHECK_COMM_COMPONENT_NULL,                                    /* 0x801e01A7 */
    SENSOR_TASK_LISTENER_FIND_NULL,                                           /* 0x801e01A8 */
    SENSOR_TASK_SENSOR_NULL,                                                  /* 0x801e01A9 */
    SENSOR_TASK_HSDATA_UPDATE_DATA_NULL,                                      /* 0x801e01AA */
    SENSOR_TASK_VSENSOR_INFO_FIND_NULL,                                       /* 0x801e01AB */
    SENSOR_UTIL_CANNOT_GET_ALG_TASK,                                           /* 0x801e01AC */
    SENSOR_VSENSOR_NOT_REGISTER,                                               /* 0x801e01AD */
    SENSOR_HSENSOR_REG_NULL,                                                   /* 0x801e01AF */
    SENSOR_TASK_HLISTENER_REG_TYPE_ERR,                                       /* 0x801e01B1 */
    SENSOR_TASK_RELY_SENSOR_NOT_REGISTERED,                                   /* 0x801e01B2 */
    SENSOR_TASK_SENSOR_DECISION_PARA_ERROR,                                   /* 0x801e01B3 */
    SENSOR_TASK_VSENSOR_OPEN_FAIL,                                            /* 0x801e01B4 */
    SENSOR_TASK_VSENSOR_CLOSE_FAIL,                                           /* 0x801e01B5 */
    SENSOR_TASK_VSENSOR_OPEN_NULL,                                            /* 0x801e01B6 */
    SENSOR_TASK_VSENSOR_CLOSE_NULL,                                           /* 0x801e01B7 */
    SENSOR_TASK_VSENSOR_PROCESS_NULL,                                         /* 0x801e01B8 */

    /* 数据管理错误码 SENSOR_ETYPE_DATA_SERVICE */
    SENSOR_DATA_SERVICE_ERRNO = SENSOR_ERR_NUMBER(SENSOR_ETYPE_DATA_SERVICE, 0), /* 0x801e0200 */
    SENSOR_SENSOR_DECISION_PARA_NULL,                                            /* 0x801e0201 */
    SENSOR_HSENSOR_CALIBRATE_INF_NULL,                                           /* 0x801e0202 */
    SENSOR_HSENSOR_SELFTEST_INF_NULL,                                            /* 0x801e0203 */
    SENSOR_HSENSOR_DRV_CALIBRATE_ERR,                                            /* 0x801e0204 */
    SENSOR_HSENSOR_DRV_SELFTEST_ERR,                                             /* 0x801e0205 */

    SENSOR_HSENSOR_PERIOD_BATCH_ERR,
    SENSOR_HSENSOR_MODE_ERR,
    SENSOR_HSENSOR_LISTENER_INFO_NULL,
    SENSOR_HSENSOR_LISTENER_NULL,
    SENSOR_HSENSOR_GET_SCB_SENSORIL,
    SENSOR_HSENSOR_SCB_NULL,
    SENSOR_HSENSOR_SCB_HSENSOR_NULL,
    SENSOR_HSENSOR_SCB_LISTENER_EXIST,
    SENSOR_HSENSOR_SCB_LISTENER_NOT_EXIST,
    SENSOR_HSENSOR_SCB_REPORT_PERIOD_ZERO,
    SENSOR_HSENSOR_NULL,
    SENSOR_HSENSOR_NAME_NULL,
    SENSOR_HSENSOR_INTERSENSORCE_NULL,
    SENSOR_HSENSOR_READ_ERR,
    SENSOR_HSENSOR_REGISTER_TYPE_DUPLICATE,

    /* SENSOR基础框架，线程，消息队列等模块 错误码 SENSOR_ETYPE_FRAME */
    SENSOR_FRAME_ERRNO = SENSOR_ERR_NUMBER(SENSOR_ETYPE_FRAME, 0), /* 0x801e0600 */
    SENSOR_MEMSET_ERR,                                             /* 0x801e0601 */
    SENSOR_SEND_TMSG_NULL,                                         /* 0x801e0602 */
    SENSOR_SEND_TMSG_MSG_QUEUE_NULL,                               /* 0x801e0603 */
    SENSOR_RECV_TMSG_NULL,                                         /* 0x801e0604 */
    SENSOR_RECV_TMSG_MSG_QUEUE_NULL,                               /* 0x801e0605 */
    SENSOR_RECV_TMSG_ERR,                                          /* 0x801e0606 */
    SENSOR_FREE_TMSG_NULL,                                         /* 0x801e0607 */
    SENSOR_MSG_QUEUE_CREATE_FAIL,                                  /* 0x801e0608 */
    SENSOR_TASK_CREATE_FAIL,                                       /* 0x801e0609 */
    SENSOR_SEND_TMSG_DEST_NOT_RUNNING,                             /* 0x801e060A */
    SENSOR_SENSOR_NOT_INIT,                                        /* 0x801e060B */
    SENSOR_SENSOR_SECUREC_FUNC_ERR,                                /* 0x801e060C */
    SENSOR_VSENSOR_TASK_CLASS_NULL,                                /* 0x801e060D */
    SENSOR_TASK_CREATE_ID_WRONG,                                   /* 0x801e060F */

    /* SENSOR MGR 所有入参检验 */
    SENSOR_ENTRY_PARA_ERROR = SENSOR_ERR_NUMBER(SENSOR_ETYPE_ENTRY_PARA, 0), /* 0x801e0900 */
    SENSOR_ENTRY_HSENSOR_NULL,                                               /* 0x801e0901 */
    SENSOR_ENTRY_HSENSOR_NAME_NULL,                                          /* 0x801e0902 */
    SENSOR_ENTRY_HSENSOR_FUNC_NULL,                                          /* 0x801e0903 */
    SENSOR_ENTRY_HSENSOR_TYPE_NULL,                                          /* 0x801e0904 */
    SENSOR_ENTRY_VSENSOR_NULL,                                               /* 0x801e0905 */
    SENSOR_ENTRY_VSENSOR_NAME_NULL,                                          /* 0x801e0906 */
    SENSOR_ENTRY_VSENSOR_FUNC_NULL,                                          /* 0x801e0907 */
    SENSOR_ENTRY_VSENSOR_TYPE_NULL,                                          /* 0x801e0908 */
    SENSOR_ENTRY_VSENSOR_RELY_NULL,                                          /* 0x801e0909 */
    SENSOR_ENTRY_VSENSOR_RELY_PARA_ERR,                                      /* 0x801e090A */
    SENSOR_ENTRY_CALIBRATE_PARA_NULL,                                        /* 0x801e090B */
    SENSOR_ENTRY_SELFTEST_PARA_NULL,                                         /* 0x801e090C */
    SENSOR_ENTRY_SENSOR_TYPE_ILLEGAL,                                        /* 0x801e090D */

    SENSOR_MAX
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

#ifndef SENSOR_MIN
#define SENSOR_MIN(a, b) (((a) > (b)) ? (b) : (a))
#endif
#ifndef SENSOR_MAX
#define SENSOR_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define CHECK_NULL_PTR_RETURN_VALUE(ptr, ret)                                        \
    do {                                                                             \
        if ((ptr) == NULL) {                                                         \
            HILOG_ERROR(HILOG_MODULE_SEN, "pointer is null and return ret:%d", ret); \
            return (ret);                                                            \
        }                                                                            \
    } while (0)

#define CHECK_NULL_PTR_RETURN(ptr)                                       \
    do {                                                                 \
        if ((ptr) == NULL) {                                             \
            HILOG_ERROR(HILOG_MODULE_SEN, "pointer is null and return"); \
            return;                                                      \
        }                                                                \
    } while (0)

#define CHECK_SENSOR_TYPE_ILLEGAL_RETURN_RET(type, ret)                \
    do {                                                               \
        if ((type == SENSOR_TYPE_NONE) || (type >= SENSOR_TYPE_MAX)) { \
            HILOG_ERROR(HILOG_MODULE_SEN, "type :%d illegal\n", type); \
            return ret;                                                \
        }                                                              \
    } while (0)

#define CHECK_SENSOR_TYPE_ILLEGAL_RETURN(type)                         \
    do {                                                               \
        if ((type == SENSOR_TYPE_NONE) || (type >= SENSOR_TYPE_MAX)) { \
            HILOG_ERROR(HILOG_MODULE_SEN, "type :%d illegal\n", type); \
            return;                                                    \
        }                                                              \
    } while (0)

#define SENSOR_CHECK_OK(x)                                         \
    do {                                                           \
        if ((int32_t)(x) != SENSOR_OK) {                           \
            HILOG_ERROR(HILOG_MODULE_SEN, "ret err ret:%d!\n", x); \
            return x;                                              \
        }                                                          \
    } while (0)
#endif /* HDI_SENSOR_COMMON_H */
