#ifndef __VFS_LFS_H__
#define __VFS_LFS_H__

#include <nuttx/fs/fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int     vfs_lfs_bind(struct inode *blkdriver, const void *data, void **handle);
int     vfs_lfs_unbind(void *handle, struct inode **blkdriver);

int     vfs_lfs_open(struct file *filep, const char *relpath, int oflags, mode_t mode);
int     vfs_lfs_close(struct file *filep);
ssize_t vfs_lfs_read(struct file *filep, char *buffer, size_t buflen);
ssize_t vfs_lfs_write(struct file *filep, const char *buffer, size_t buflen);
off_t   vfs_lfs_seek(struct file *filep, off_t offset, int whence);
off64_t vfs_lfs_seek64(struct file *filep, off64_t offset, int whence);
int     vfs_lfs_truncate(struct file *filep, off_t length);
int     vfs_lfs_sync(struct file *filep);

int     vfs_lfs_opendir(struct inode *mountpt, const char *relpath, struct fs_dirent_s *dir);
int     vfs_lfs_closedir(struct inode *mountpt, struct fs_dirent_s *dir);
int     vfs_lfs_readdir(struct inode *mountpt, struct fs_dirent_s *dir);
int     vfs_lfs_rewinddir(struct inode *mountpt, struct fs_dirent_s *dir);

int     vfs_lfs_unlink(struct inode *mountpt, const char *relpath);
int     vfs_lfs_mkdir(struct inode *mountpt, const char *relpath, mode_t mode);
int     vfs_lfs_rmdir(struct inode *mountpt, const char *relpath);
int     vfs_lfs_rename(struct inode *mountpt, const char *oldrelpath, const char *newrelpath);
int     vfs_lfs_stat(struct inode *mountpt, const char *relpath, struct stat *buf);
int     vfs_lfs_statfs(struct inode *mountpt, struct statfs *buf);

#ifdef __cplusplus
}
#endif

#endif /* __VFS_LFS_H__ */
