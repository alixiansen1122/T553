#include "lfs.h"
#include "vfs_fs.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "littlefs_cfg.h"
#include "lfs_log.h"

/* 来自 littlefs_port.c（单分区版） */
extern void  littlefs_init(void);
extern int   littlefs_mount(int auto_format);
extern void  littlefs_unmount(void);
extern lfs_t* littlefs_get(void);

/* =========================================
   文件 / 目录 私有结构（简化：不再需要 pid）
======================================== */

typedef struct {
    lfs_file_t file;
} vfs_lfs_file_t;

typedef struct {
    lfs_dir_t dir;
} vfs_lfs_dir_t;

/* =========================================
   工具函数
======================================== */

static int convert_flags(int oflags)
{
    int flags = 0;
    if (oflags & O_RDONLY) flags |= LFS_O_RDONLY;
    if (oflags & O_WRONLY) flags |= LFS_O_WRONLY;
    if (oflags & O_RDWR)   flags |= LFS_O_RDWR;
    if (oflags & O_CREAT)  flags |= LFS_O_CREAT;
    if (oflags & O_EXCL)   flags |= LFS_O_EXCL;
    if (oflags & O_TRUNC)  flags |= LFS_O_TRUNC;
    if (oflags & O_APPEND) flags |= LFS_O_APPEND;

    LFS_LOG_INFO("convert_flags: oflags=0x%x -> lfs_flags=0x%x", oflags, flags);
    return flags;
}

/* =========================================
   mount / unmount
======================================== */

int vfs_lfs_bind(struct inode *blkdriver,
                 const void *data,
                 void **handle)
{
    (void)blkdriver;
    (void)data;

    LFS_LOG_INFO("vfs_lfs_bind start");

    littlefs_init();

    if (littlefs_mount(1) != 0) {
        LFS_LOG_ERR("littlefs_mount failed");
        return -EIO;
    }

    LFS_LOG_INFO("littlefs mounted");

    *handle = (void *)1;
    return 0;
}

int vfs_lfs_unbind(void *handle,
                   struct inode **blkdriver)
{
    (void)handle;
    (void)blkdriver;

    littlefs_unmount();
    LFS_LOG_INFO("littlefs unmounted");

    return 0;
}

/* =========================================
   文件操作
======================================== */

int vfs_lfs_open(struct file *filep,
                 const char *relpath,
                 int oflags,
                 mode_t mode)
{
    (void)mode;

    LFS_LOG_INFO("open: %s flags=0x%x", relpath, oflags);

    lfs_t *lfs = littlefs_get();
    if (!lfs) {
        LFS_LOG_ERR("open failed: lfs not mounted");
        return -EINVAL;
    }

    vfs_lfs_file_t *fp = malloc(sizeof(vfs_lfs_file_t));
    if (!fp) {
        LFS_LOG_ERR("malloc file struct failed");
        return -ENOMEM;
    }

    int flags = convert_flags(oflags);
    int ret = lfs_file_open(lfs, &fp->file, relpath, flags);
    if (ret < 0) {
        LFS_LOG_ERR("lfs_file_open failed ret=%d", ret);
        free(fp);
        return ret;
    }

    filep->f_priv = fp;

    LFS_LOG_INFO("open success: %s", relpath);
    return 0;
}

int vfs_lfs_close(struct file *filep)
{
    vfs_lfs_file_t *fp = filep->f_priv;
    if (!fp) return -EINVAL;

    LFS_LOG_INFO("close file");

    int ret = lfs_file_close(littlefs_get(), &fp->file);
    free(fp);

    if (ret < 0)
        LFS_LOG_ERR("close failed ret=%d", ret);

    return ret;
}

ssize_t vfs_lfs_read(struct file *filep, char *buffer, size_t buflen)
{
    vfs_lfs_file_t *fp = filep->f_priv;
    ssize_t ret = lfs_file_read(littlefs_get(), &fp->file, buffer, buflen);

    if (ret < 0){
        LFS_LOG_ERR("read failed ret=%d", (int)ret);
    }
    else{
        filep->f_pos += ret;
        LFS_LOG_INFO("read %d bytes", (int)ret);
    }

    return ret;
}

ssize_t vfs_lfs_write(struct file *filep, const char *buffer, size_t buflen)
{
    vfs_lfs_file_t *fp = filep->f_priv;
    ssize_t ret = lfs_file_write(littlefs_get(), &fp->file, buffer, buflen);

    if (ret < 0){
        LFS_LOG_ERR("write failed ret=%d", (int)ret);
    }
    else{
        filep->f_pos += ret;
        LFS_LOG_INFO("write %d bytes", (int)ret);
    }

    return ret;
}

off_t vfs_lfs_seek(struct file *filep, off_t offset, int whence)
{
    vfs_lfs_file_t *fp = filep->f_priv;

    off_t ret = lfs_file_seek(littlefs_get(), &fp->file, offset, whence);

    if (ret >= 0) {
        filep->f_pos = ret;
    }

    LFS_LOG_INFO("[LFS_SEEK] file=%s, offset=%ld, whence=%d, result=%ld, f_pos=%ld\n",
           filep->f_relpath, (long)offset, whence, (long)ret, (long)filep->f_pos);

    return ret;
}


int vfs_lfs_truncate(struct file *filep, off_t length)
{
    vfs_lfs_file_t *fp = filep->f_priv;
    int ret = lfs_file_truncate(littlefs_get(), &fp->file, length);

    if (ret < 0)
        LFS_LOG_ERR("truncate failed ret=%d", ret);
    else
        LFS_LOG_INFO("truncate success len=%ld", (long)length);

    return ret;
}

/* =========================================
   目录操作
======================================== */

int vfs_lfs_opendir(struct inode *mountpt,
                    const char *relpath,
                    struct fs_dirent_s *dir)
{
    (void)mountpt;

    LFS_LOG_INFO("opendir: %s", relpath);

    lfs_t *lfs = littlefs_get();
    if (!lfs) {
        LFS_LOG_ERR("opendir: lfs not mounted");
        return -EINVAL;
    }

    vfs_lfs_dir_t *d = malloc(sizeof(vfs_lfs_dir_t));
    if (!d) {
        LFS_LOG_ERR("malloc dir struct failed");
        return -ENOMEM;
    }

    int ret = lfs_dir_open(lfs, &d->dir, relpath);
    if (ret < 0) {
        LFS_LOG_ERR("lfs_dir_open failed ret=%d", ret);
        free(d);
        return ret;
    }

    dir->u.fs_dir = d;
    return 0;
}

int vfs_lfs_closedir(struct inode *mountpt, struct fs_dirent_s *dir)
{
    (void)mountpt;

    vfs_lfs_dir_t *d = dir->u.fs_dir;
    if (!d) return -EINVAL;

    LFS_LOG_INFO("closedir");

    int ret = lfs_dir_close(littlefs_get(), &d->dir);
    free(d);

    return ret;
}

int vfs_lfs_readdir(struct inode *mountpt, struct fs_dirent_s *dir)
{
    (void)mountpt;

    vfs_lfs_dir_t *d = dir->u.fs_dir;
    struct lfs_info info;

    int ret = lfs_dir_read(littlefs_get(), &d->dir, &info);
    if (ret <= 0)
        return ret;

    strcpy(dir->fd_dir.d_name, info.name);

    LFS_LOG_INFO("readdir: %s", info.name);
    return 0;
}

int vfs_lfs_rewinddir(struct inode *mountpt, struct fs_dirent_s *dir)
{
    (void)mountpt;

    vfs_lfs_dir_t *d = dir->u.fs_dir;
    lfs_dir_rewind(littlefs_get(), &d->dir);

    LFS_LOG_INFO("rewinddir");
    return 0;
}

/* =========================================
   文件系统操作
======================================== */

int vfs_lfs_unlink(struct inode *mountpt, const char *relpath)
{
    (void)mountpt;

    LFS_LOG_INFO("unlink: %s", relpath);

    lfs_t *lfs = littlefs_get();
    if (!lfs) return -EINVAL;

    return lfs_remove(lfs, relpath);
}

int vfs_lfs_mkdir(struct inode *mountpt, const char *relpath, mode_t mode)
{
    (void)mountpt;
    (void)mode;

    LFS_LOG_INFO("mkdir: %s", relpath);

    lfs_t *lfs = littlefs_get();
    if (!lfs) return -EINVAL;

    return lfs_mkdir(lfs, relpath);
}

int vfs_lfs_rename(struct inode *mountpt,
                   const char *oldrelpath,
                   const char *newrelpath)
{
    (void)mountpt;

    LFS_LOG_INFO("rename: %s -> %s", oldrelpath, newrelpath);

    lfs_t *lfs = littlefs_get();
    if (!lfs) return -EINVAL;

    return lfs_rename(lfs, oldrelpath, newrelpath);
}

int vfs_lfs_stat(struct inode *mountpt,
                 const char *relpath,
                 struct stat *buf)
{
    (void)mountpt;

    lfs_t *lfs = littlefs_get();
    if (!lfs) return -EINVAL;

    struct lfs_info info;
    int ret = lfs_stat(lfs, relpath, &info);
    if (ret < 0) {
        LFS_LOG_ERR("stat failed ret=%d, path=%s", ret, relpath);
        return ret;
    }

    memset(buf, 0, sizeof(struct stat));
    buf->st_size = info.size;

    if (info.type == LFS_TYPE_DIR) {
        /* ★ 目录：rwxrwxrwx */
        buf->st_mode = S_IFDIR | 0777;
    } else {
        /* ★ 文件：rw-rw-rw- */
        buf->st_mode = S_IFREG | 0666;
    }

    /* 补充其他字段，部分系统调用可能会用到 */
    buf->st_blksize = LFS_BLOCK_SIZE;
    buf->st_blocks  = (info.size + LFS_BLOCK_SIZE - 1) / LFS_BLOCK_SIZE;

    LFS_LOG_INFO("stat: %s size=%ld mode=0%o",
                 relpath, (long)info.size, buf->st_mode);
    return 0;
}

int vfs_lfs_statfs(struct inode *mountpt, struct statfs *buf)
{
    (void)mountpt;

    lfs_t *lfs = littlefs_get();
    if (!lfs) return -EINVAL;

    lfs_ssize_t used = lfs_fs_size(lfs);
    if (used < 0) {
        LFS_LOG_ERR("lfs_fs_size failed ret=%d", (int)used);
        return used;
    }

    memset(buf, 0, sizeof(struct statfs));
    buf->f_bsize  = LFS_BLOCK_SIZE;
    buf->f_blocks = USER_BLOCK_COUNT;
    buf->f_bfree  = USER_BLOCK_COUNT - used;

    LFS_LOG_INFO("statfs: used=%d free=%d",
                 (int)used, (int)(buf->f_bfree));
    return 0;
}

/* =========================================
   扩展接口
======================================== */

int vfs_lfs_sync(struct file *filep)
{
    vfs_lfs_file_t *fp = filep->f_priv;
    if (!fp) return -EINVAL;

    int ret = lfs_file_sync(littlefs_get(), &fp->file);

    if (ret < 0)
        LFS_LOG_ERR("sync failed ret=%d", ret);
    else
        LFS_LOG_INFO("sync success");

    return ret;
}

off64_t vfs_lfs_seek64(struct file *filep,
                       off64_t offset,
                       int whence)
{
    return (off64_t)vfs_lfs_seek(filep, (off_t)offset, whence);
}

int vfs_lfs_rmdir(struct inode *mountpt, const char *relpath)
{
    (void)mountpt;
    LFS_LOG_INFO("rmdir: %s", relpath);
    return vfs_lfs_unlink(mountpt, relpath);
}
