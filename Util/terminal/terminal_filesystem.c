//
// Created by caesar kekxv on 2020/9/26.
//

#include "terminal_filesystem.h"
#include "flash_rw.h"

extern TerminalUserInfo terminalUserInfo;
uint32_t flash_offset = 63;
lfs_t Terminal_lfs_roots;
lfs_dir_t Terminal_lfs_dirs;
// configuration of the filesystem is provided by this struct
struct lfs_config Terminal_cfg = {
        .context = NULL,
        // block device operations
        .read = Terminal_provided_block_device_read, .prog = Terminal_provided_block_device_prog, .erase = Terminal_provided_block_device_erase, .sync = Terminal_provided_block_device_sync,

        // block device configuration
        .read_size = 16, .prog_size = 16, .block_size = 1024,
        // .block_size = (((*(uint16_t *) FLASHSIZE_BASE) > 128) ? 2048 : 1024),
        .block_count = TERMINAL_LFS_BUFF_COUNT, .block_cycles = 100, .cache_size = 16, .lookahead_size = 16, .read_buffer = NULL, .prog_buffer = NULL, .lookahead_buffer = NULL, .name_max = 0, .file_max = 0, .attr_max = 0
};

void Terminal_Lfs_Init() {
    Terminal_cfg.block_size = FLASH_RW_PAGESIZE;
    flash_offset = (*(uint16_t *) FLASHSIZE_BASE) - TERMINAL_LFS_BUFF_COUNT - 1;
    // mount the filesystem
    int err = lfs_mount(&Terminal_lfs_roots, &Terminal_cfg);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        err = lfs_format(&Terminal_lfs_roots, &Terminal_cfg);
        if (LFS_ERR_OK == err) {
            err = lfs_mount(&Terminal_lfs_roots, &Terminal_cfg);
        }
    }

    if (LFS_ERR_OK == err) {
        Terminal_Mkdir("/root");
        Terminal_Mkdir("/home");
        Terminal_Mkdir("/bin");
        Terminal_Mkdir("/data");
    }
    Terminal_ChangeDir("/");
}

int Terminal_ChangeDir(const char *path) {
    lfs_dir_close(&Terminal_lfs_roots, &Terminal_lfs_dirs);
    int err = lfs_dir_open(&Terminal_lfs_roots, &Terminal_lfs_dirs, path);
    if (0 == err) {
        memcpy(terminalUserInfo.Path, path, strlen(path));
        terminalUserInfo.Path[strlen(path)] = 0;
    }
    return err;
}

int Terminal_ListsDir(const char *path) {
    lfs_dir_t dir;
    int err = lfs_dir_open(&Terminal_lfs_roots, &dir, path);
    if (err) {
        return err;
    }
    Terminal_ForeColor(enmCFC_Cyan);
    Terminal_Printf("Path : ");
    Terminal_Printf(Terminal_GetDirPathThe(path));
    Terminal_ResetColor();
    Terminal_Printf("\n");
    struct lfs_info info;
    while (true) {
        int res = lfs_dir_read(&Terminal_lfs_roots, &dir, &info);
        if (res < 0) {
            return res;
        }
        if (res == 0) {
            break;
        }
        switch (info.type) {
            case LFS_TYPE_REG:
                Terminal_Printf("reg ");
                break;
            case LFS_TYPE_DIR:
                Terminal_Printf("dir ");
                break;
            default:
                Terminal_Printf("?   ");
                break;
        }

        char da[10] = {};
        static const char *prefixes[] = {"", "K", "M", "G"};
        for (int i = sizeof(prefixes) / sizeof(prefixes[0]) - 1; i >= 0; i--) {
            if (info.size >= (1 << 10 * i) - 1) {
                // printf("%*u%sB ", 4-(i != 0), info.size >> 10*i, prefixes[i]);
                itoa(info.size >> 10 * i, da, 10);
                for (int i = strlen(da); i < 4; i++) {
                    Terminal_Printf(" ");
                }
                Terminal_Printf(da);
                Terminal_Printf(prefixes[i]);
                Terminal_Printf("B ");
                break;
            }
        }

        if (LFS_TYPE_DIR == info.type) {
            Terminal_ForeColor(enmCFC_Cyan);
        }
        Terminal_Printf(info.name);
        Terminal_ResetColor();
        Terminal_Printf("\n");
    }

    err = lfs_dir_close(&Terminal_lfs_roots, &dir);
    if (err) {
        return err;
    }

    return 0;
}


/**
 * 查看文件系统大小
 * @return
 */
int Terminal_FileSystemSize() {
    lfs_ssize_t ret = lfs_fs_size(&Terminal_lfs_roots);
    if (ret < 0) {
        return ret;
    }
    Terminal_ForeColor(enmCFC_Cyan);
    Terminal_Printf("Path [/] : ");
    Terminal_ResetColor();
    Terminal_PrintfNum(ret * Terminal_lfs_roots.cfg->block_size);
    Terminal_Printf("b \n");
    return ret;
}

#ifdef WIN32
#define IS_SLASH(s) ((s == '/') || (s == '\\'))
#else
#define IS_SLASH(s) (s == '/')
#endif

void Terminal_ap_getparents(char *name) {
    char *next;
    int l, w, first_dot;

    /* Four paseses,as per RFC 1808 */
    /* a) remove ./ path segments */
    for (next = name; *next && (*next != '.'); next++) {
    }

    l = w = first_dot = next - name;
    while (name[l] != '\0') {
        if (name[l] == '.' && IS_SLASH(name[l + 1]) && (l == 0 || IS_SLASH(name[l - 1])))
            l += 2;
        else
            name[w++] = name[l++];
    }

    /* b) remove trailing . path,segment */
    if (w == 1 && name[0] == '.')
        w--;
    else if (w > 1 && name[w - 1] == '.' && IS_SLASH(name[w - 2]))
        w--;
    name[w] = '\0';

    /* c) remove all xx/../ segments. (including leading ../ and /../) */
    l = first_dot;

    while (name[l] != '\0') {
        if (name[l] == '.' && name[l + 1] == '.' && IS_SLASH(name[l + 2]) && (l == 0 || IS_SLASH(name[l - 1]))) {
            int m = l + 3, n;

            l = l - 2;
            if (l >= 0) {
                while (l >= 0 && !IS_SLASH(name[l]))
                    l--;
                l++;
            }
            else
                l = 0;
            n = l;
            while ((name[n] = name[m]))
                (++n, ++m);
        }
        else
            ++l;
    }

    /* d) remove trailing xx/.. segment. */
    if (l == 2 && name[0] == '.' && name[1] == '.')
        name[0] = '\0';
    else if (l > 2 && name[l - 1] == '.' && name[l - 2] == '.' && IS_SLASH(name[l - 3])) {
        l = l - 4;
        if (l >= 0) {
            while (l >= 0 && !IS_SLASH(name[l]))
                l--;
            l++;
        }
        else
            l = 0;
        name[l] = '\0';
    }
}

int Terminal_GetDirSystemRoots(const char *path, char *_path) {
    // if (path[0] == '/') {
    // }
    int len = 0;
    if (strlen(path) == 0) {
        len = strlen(terminalUserInfo.Path);
        memcpy(_path, terminalUserInfo.Path, len);
    }
    else if (path[0] != '/') {
        len = strlen(terminalUserInfo.Path);
        memcpy(_path, terminalUserInfo.Path, len);
        if (_path[len - 1] != '/') {
            _path[len++] = '/';
        }
        memcpy(&_path[len], path, strlen(path));
        len += strlen(path);
    }
    else {
        memcpy(_path, path, strlen(path));
        len = strlen(path);
    }
    _path[len] = 0;
    Terminal_ap_getparents(_path);
    len = strlen(_path);
    if (0 != strcmp("/", _path) && _path[len - 1] == '/') {
        _path[--len] = 0;
    }
    return len;
}

int Terminal_ChangeDirSystemRoots(const char *path) {
    char _path[MAX_PATH_SIZE];
    Terminal_GetDirSystemRoots(path, _path);
    return Terminal_ChangeDir(_path);
}

int Terminal_ListsDirSystemRoots(const char *path) {
    char _path[MAX_PATH_SIZE];
    Terminal_GetDirSystemRoots(path, _path);
    return Terminal_ListsDir(_path);
}

const char *Terminal_GetDirPath() {
    return Terminal_GetDirPathThe(terminalUserInfo.Path);
}

const char *Terminal_GetDirPathThe(const char *path) {
    return path;
}

int Terminal_Mkdir(const char *path) {
    struct lfs_info _lfs_info;
    char _path[MAX_PATH_SIZE] = {};
    Terminal_GetDirSystemRoots(path, _path);
    int err = lfs_stat(&Terminal_lfs_roots, _path, &_lfs_info);
    if (err) {
        err = lfs_mkdir(&Terminal_lfs_roots, _path);
    }
    return err;
}

int Terminal_Remove(const char *path) {
    struct lfs_info _lfs_info;
    char _path[MAX_PATH_SIZE] = {};
    Terminal_GetDirSystemRoots(path, _path);
    int err = lfs_stat(&Terminal_lfs_roots, _path, &_lfs_info);
    if (!err) {
        err = lfs_remove(&Terminal_lfs_roots, _path);
    }
    return err;
}

int Terminal_Rename(const char *path, char *new_path) {
    struct lfs_info _lfs_info;
    char _path[MAX_PATH_SIZE] = {}, _new_path[MAX_PATH_SIZE] = {};
    Terminal_GetDirSystemRoots(path, _path);
    Terminal_GetDirSystemRoots(new_path, _new_path);
    int err = lfs_stat(&Terminal_lfs_roots, _path, &_lfs_info);
    if (!err) {
        err = lfs_rename(&Terminal_lfs_roots, _path, _new_path);
    }
    else {
        return -1;
    }
    return err;
}

/**
 * touch 命令 创建文件
 * @param path
 * @return
 */
int Terminal_CreateFile(const char *path) {
    struct lfs_info _lfs_info;
    char _path[MAX_PATH_SIZE] = {};
    Terminal_GetDirSystemRoots(path, _path);
    int err = lfs_stat(&Terminal_lfs_roots, _path, &_lfs_info);
    if (err) {
        lfs_file_t file;
        err = lfs_file_open(&Terminal_lfs_roots, &file, path,
                            LFS_O_WRONLY | LFS_O_CREAT | LFS_O_EXCL);
        if (!err) {
            lfs_file_close(&Terminal_lfs_roots, &file);
        }
    }
    else {
        return -1;
    }
    return err;
}

/**
 * 打印文件内容
 * @param path
 * @return
 */
int Terminal_CatFile(const char *path) {
    struct lfs_info _lfs_info;
    char _path[MAX_PATH_SIZE] = {};
    Terminal_GetDirSystemRoots(path, _path);
    int err = lfs_stat(&Terminal_lfs_roots, _path, &_lfs_info);
    if (err) {
        return -1;
    }
    lfs_file_t file;
    err = lfs_file_open(&Terminal_lfs_roots, &file, path,
                        LFS_O_RDONLY);
    if (err) {
        return err;
    }
    lfs_soff_t file_size = lfs_file_size(&Terminal_lfs_roots, &file);
    if (file_size <= 0) {
        lfs_file_close(&Terminal_lfs_roots, &file);
        return file_size;
    }
    char buffer[3];
    for (lfs_size_t i = 0; i < file_size; i += 2) {
        lfs_size_t chunk = lfs_min(2, file_size - i);
        memset(buffer, 0x00, 3);
        lfs_size_t ret = lfs_file_read(&Terminal_lfs_roots, &file, buffer, chunk);
        if (chunk != ret) {
            lfs_file_close(&Terminal_lfs_roots, &file);
            return ret;
        }
        for (int _ = 0; _ < ret; _++) {
            Terminal_PrintfChar(buffer[_]);
        }
    }

    lfs_file_close(&Terminal_lfs_roots, &file);
    return err;
}

int Terminal_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
                                        lfs_off_t offset, void *buffer, lfs_size_t size) {
    // memcpy(buffer, &Terminal_file_Buff[block][offset], size);
    FLASH_RW_Read(flash_offset + block, buffer, size, offset);
    return 0;
}

int Terminal_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
                                        lfs_off_t offset, const void *buffer, lfs_size_t size) {
    HAL_StatusTypeDef ret = HAL_OK;
    // memcpy(&Terminal_file_Buff[block][offset], buffer, size);
    ret = FLASH_RW_Write(flash_offset + block, buffer, size, offset);
    if (HAL_OK == ret) {
        return 0;
    }
    else {
        return LFS_ERR_CORRUPT;
    }
}

int Terminal_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    HAL_StatusTypeDef ret = HAL_OK;
    // memset(Terminal_file_Buff[block], 0x00, sizeof(Terminal_file_Buff[block]));
    ret = FLASH_RW_erase(flash_offset + block);
    if (HAL_OK == ret) {
        return 0;
    }
    else {
        return LFS_ERR_CORRUPT;
    }
}

int Terminal_provided_block_device_sync(const struct lfs_config *c) {
    return 0;
}
