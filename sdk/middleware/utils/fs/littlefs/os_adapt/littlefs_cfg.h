/* littlefs_cfg.h */

#ifndef __LITTLEFS_CFG_H__
#define __LITTLEFS_CFG_H__

/* ========= Flash 基本参数 ========= */

/* SPI NOR 物理参数 */
#define SPI_NOR_TOTAL_SIZE        (16 * 1024 * 1024)   /* 16MB */
#define SPI_NOR_SECTOR_SIZE       4096
#define SPI_NOR_PAGE_SIZE         256

/* ========= 分区规划 ========= */

/* 文件系统使用最后 6MB */
#define LITTLEFS_SIZE             (6 * 1024 * 1024)

/* LittleFS 起始地址 = 16MB - 6MB = 10MB */
#define LITTLEFS_START_ADDR       (SPI_NOR_TOTAL_SIZE - LITTLEFS_SIZE)

/* ========= LittleFS 参数 ========= */

#define LFS_READ_SIZE             SPI_NOR_PAGE_SIZE
#define LFS_PROG_SIZE             SPI_NOR_PAGE_SIZE
#define LFS_BLOCK_SIZE            SPI_NOR_SECTOR_SIZE
#define LFS_BLOCK_COUNT           (LITTLEFS_SIZE / LFS_BLOCK_SIZE)

#define LFS_CACHE_SIZE            512
#define LFS_LOOKAHEAD_SIZE        256
#define LFS_BLOCK_CYCLES          500

#define FLASH_ID                  0

/* 单分区地址与块数（供 port 层使用） */
#define USER_ADDR                 LITTLEFS_START_ADDR
#define USER_BLOCK_COUNT          LFS_BLOCK_COUNT

/* ========= 安全校验 ========= */

#if (USER_ADDR + LITTLEFS_SIZE) > SPI_NOR_TOTAL_SIZE
#error "LittleFS overflow flash!"
#endif

#if (LITTLEFS_SIZE % SPI_NOR_SECTOR_SIZE) != 0
#error "LITTLEFS_SIZE must be sector-aligned!"
#endif

#endif /* __LITTLEFS_CFG_H__ */
