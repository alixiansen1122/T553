#ifndef _LFS_LOG_H_
#define _LFS_LOG_H_

#include <stdio.h>

#ifndef LFS_LOG_LEVEL
#define LFS_LOG_LEVEL 1 /* 1:ERR 2:WARN 3:INFO */
#endif

#define LFS_LOG_ERR(fmt, ...)  do { printf("[ERR] " fmt "\n", ##__VA_ARGS__); } while(0)
#define LFS_LOG_WARN(fmt, ...) do { if (LFS_LOG_LEVEL >= 2) printf("[WARN] " fmt "\n", ##__VA_ARGS__); } while(0)
#define LFS_LOG_INFO(fmt, ...) do { if (LFS_LOG_LEVEL >= 3) printf("[INFO] " fmt "\n", ##__VA_ARGS__); } while(0)

#endif