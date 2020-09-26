//
// Created by caesar kekxv on 2020/9/26.
//

#ifndef KOS_TERMINAL_FILESYSTEM_H
#define KOS_TERMINAL_FILESYSTEM_H

#include "lfs.h"
#include "lfs_util.h"
#include "main.h"
#include "terminal.h"

/* ****************
 * 文件系统
 * ***************/

// variables used by the filesystem
extern lfs_t Terminal_lfs_roots;
extern lfs_dir_t Terminal_lfs_dirs;

// configuration of the filesystem is provided by this struct
extern struct lfs_config Terminal_cfg;

/**
 * 初始化文件系统
 */
void Terminal_Lfs_Init();

const char *Terminal_GetDirPath();

const char *Terminal_GetDirPathThe(const char *path);

/**
 * 切换当前目录
 * @param path
 * @return
 */
int Terminal_ChangeDir(const char *path);

/**
 * ls 命令
 * @param path
 * @return
 */
int Terminal_ListsDir(const char *path);

/**
 * 查看文件系统大小
 * @return
 */
int Terminal_FileSystemSize();
/**
 * cd 命令
 * @param path
 * @return
 */
int Terminal_ChangeDirSystemRoots(const char *path);

/**
 * 获取 绝对路径
 * @param name
 */
void Terminal_ap_getparents(char *name);

/**
 * 处理路径
 * @param path
 * @param _path
 * @return
 */
int Terminal_GetDirSystemRoots(const char *path, char *_path);

/**
 * ls 命令
 * @param path
 * @return
 */
int Terminal_ListsDirSystemRoots(const char *path);

/**
 * 创建目录
 * @param path
 * @return
 */
int Terminal_Mkdir(const char *path);
/**
 * touch 命令 创建文件
 * @param path
 * @return
 */
int Terminal_CreateFile(const char *path);
/**
 * 打印文件内容
 * @param path
 * @return
 */
int Terminal_CatFile(const char *path);

/**
 * rm 命令 删除文件或者删除目录
 * @param path
 * @return
 */
int Terminal_Remove(const char *path);

/**
 * mv 命令 重命名或者移动
 * @param path
 * @param new_path
 * @return
 */
int Terminal_Rename(const char *path, char *new_path);

/**
 * littlefs 读回调实现
 * @param c
 * @param block
 * @param off
 * @param buffer
 * @param size
 * @return
 */
int Terminal_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
                                        lfs_off_t off, void *buffer, lfs_size_t size);

/**
 * littlefs 写回调实现
 * @param c
 * @param block
 * @param off
 * @param buffer
 * @param size
 * @return
 */
int Terminal_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
                                        lfs_off_t off, const void *buffer, lfs_size_t size);

/**
 * littlefs 擦除回调实现
 * @param c
 * @param block
 * @return
 */
int Terminal_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block);

/**
 * 同步回调实现
 * @param c
 * @return
 */
int Terminal_provided_block_device_sync(const struct lfs_config *c);

/* ****************
 * 文件系统
 * ***************/


#endif //KOS_TERMINAL_FILESYSTEM_H
