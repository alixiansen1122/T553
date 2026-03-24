#include "lfs.h"
#include <string.h>
#include "littlefs_cfg.h"
#include "soc_osal.h"
#include "lfs_log.h"
#include "non_os.h"

#define FLASH_ID 0

/* ================================
   NOR 驱动接口
================================ */

extern uint32_t uapi_flash_read_data(int id, uint32_t addr, void *buf, uint32_t size);
extern uint32_t uapi_flash_write_data(int id, uint32_t addr, const void *buf, uint32_t size);
extern int uapi_flash_block_erase(int id, uint32_t addr, uint32_t size, bool wait);
extern int uapi_flash_exit_from_xip_mode(int id);
extern int uapi_flash_switch_to_xip_mode(int id);

/* ================================
   单分区实例
================================ */

typedef struct {
    lfs_t lfs;
    struct lfs_config cfg;
    osal_mutex mutex;
    uint32_t base_addr;
} lfs_instance_t;

static lfs_instance_t g_lfs;

/* ================================
   BD 接口
================================ */

static int bd_read(const struct lfs_config *c,
                   lfs_block_t block,
                   lfs_off_t off,
                   void *buffer,
                   lfs_size_t size)
{
    lfs_instance_t *inst = c->context;
    int ret = LFS_ERR_OK;

    uint32_t flash_offset =
        inst->base_addr +
        block * c->block_size +
        off;

    non_os_enter_critical();

    if (uapi_flash_exit_from_xip_mode(0) != LFS_ERR_OK) {
        ret = LFS_ERR_IO;
        goto end;
    }

    uint32_t read_len =
        uapi_flash_read_data(0, flash_offset, buffer, size);

    if (read_len != size) {
        LFS_LOG_ERR("flash read failed addr=0x%08x size=%u",
                     flash_offset, size);
        ret = LFS_ERR_IO;
    }

end:
    uapi_flash_switch_to_xip_mode(0);
    non_os_exit_critical();
    return ret;
}

static int bd_prog(const struct lfs_config *c,
                   lfs_block_t block,
                   lfs_off_t off,
                   const void *buffer,
                   lfs_size_t size)
{
    lfs_instance_t *inst = c->context;
    int ret = LFS_ERR_OK;

    uint32_t flash_offset =
        inst->base_addr +
        block * c->block_size +
        off;

    non_os_enter_critical();

    if (uapi_flash_exit_from_xip_mode(0) != LFS_ERR_OK) {
        ret = LFS_ERR_IO;
        goto end;
    }

    uint32_t write_len =
        uapi_flash_write_data(0, flash_offset, buffer, size);

    if (write_len != size) {
        LFS_LOG_ERR("flash write failed addr=0x%08x size=%u",
                     flash_offset, size);
        ret = LFS_ERR_IO;
    }

end:
    uapi_flash_switch_to_xip_mode(0);
    non_os_exit_critical();
    return ret;
}

static int bd_erase(const struct lfs_config *c,
                    lfs_block_t block)
{
    lfs_instance_t *inst = c->context;
    int ret = LFS_ERR_OK;

    uint32_t flash_offset =
        inst->base_addr +
        block * c->block_size;

    non_os_enter_critical();

    if (uapi_flash_exit_from_xip_mode(0) != LFS_ERR_OK) {
        ret = LFS_ERR_IO;
        goto end;
    }

    if (uapi_flash_block_erase(0, flash_offset, c->block_size, true) != 0) {
        LFS_LOG_ERR("flash erase failed addr=0x%08x", flash_offset);
        ret = LFS_ERR_IO;
    }

end:
    uapi_flash_switch_to_xip_mode(0);
    non_os_exit_critical();
    return ret;
}

static int bd_sync(const struct lfs_config *c)
{
    (void)c;
    return 0;
}

static int bd_lock(const struct lfs_config *c)
{
    lfs_instance_t *inst = c->context;
    osal_mutex_lock(&inst->mutex);
    return 0;
}

static int bd_unlock(const struct lfs_config *c)
{
    lfs_instance_t *inst = c->context;
    osal_mutex_unlock(&inst->mutex);
    return 0;
}

/* ================================
   对外接口
================================ */

void littlefs_init(void)
{
    LFS_LOG_INFO("littlefs init start");

    memset(&g_lfs, 0, sizeof(g_lfs));
    osal_mutex_init(&g_lfs.mutex);

    g_lfs.base_addr = USER_ADDR;

    g_lfs.cfg.context        = &g_lfs;
    g_lfs.cfg.read_size      = LFS_READ_SIZE;
    g_lfs.cfg.prog_size      = LFS_PROG_SIZE;
    g_lfs.cfg.block_size     = LFS_BLOCK_SIZE;
    g_lfs.cfg.block_count    = USER_BLOCK_COUNT;
    g_lfs.cfg.cache_size     = LFS_CACHE_SIZE;
    g_lfs.cfg.lookahead_size = LFS_LOOKAHEAD_SIZE;
    g_lfs.cfg.block_cycles   = LFS_BLOCK_CYCLES;

    g_lfs.cfg.read   = bd_read;
    g_lfs.cfg.prog   = bd_prog;
    g_lfs.cfg.erase  = bd_erase;
    g_lfs.cfg.sync   = bd_sync;
    g_lfs.cfg.lock   = bd_lock;
    g_lfs.cfg.unlock = bd_unlock;

    LFS_LOG_INFO("littlefs init done, base=0x%08x blocks=%u",
                 g_lfs.base_addr, USER_BLOCK_COUNT);
}

int littlefs_mount(int auto_format)
{
    int err = lfs_mount(&g_lfs.lfs, &g_lfs.cfg);

    if (err) {
        LFS_LOG_WARN("mount failed (%d)", err);

        if (auto_format) {
            LFS_LOG_INFO("formatting ...");

            err = lfs_format(&g_lfs.lfs, &g_lfs.cfg);
            if (err) {
                LFS_LOG_ERR("format failed (%d)", err);
                return err;
            }

            err = lfs_mount(&g_lfs.lfs, &g_lfs.cfg);
            if (err) {
                LFS_LOG_ERR("mount failed after format (%d)", err);
                return err;
            }
        } else {
            return err;
        }
    }

    LFS_LOG_INFO("mount success");
    return 0;
}

void littlefs_unmount(void)
{
    lfs_unmount(&g_lfs.lfs);
    LFS_LOG_INFO("unmounted");
}

lfs_t* littlefs_get(void)
{
    return &g_lfs.lfs;
}
