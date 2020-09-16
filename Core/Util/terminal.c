//
// Created by caesar kekxv on 2020/9/14.
//

#include <stdlib.h>
#include <stdbool.h>
#include "util.h"
#include "string.h"
#include "main.h"
#include "terminal.h"
#include "terminal_command.h"

TerminalUserInfo terminalUserInfo = {};

int8_t Terminal_offset = 0;
char Terminal_command[TERMINAL_COMMAND_MAX_LENGTH] = {};

Terminal_getc Terminal_getc_cb = NULL;
Terminal_putc Terminal_putc_cb = NULL;

lfs_t Terminal_lfs_roots;
lfs_dir_t Terminal_lfs_dirs;
struct lfs_info Terminal_lfs_info;
// configuration of the filesystem is provided by this struct
const struct lfs_config Terminal_cfg = {
        .context = NULL,
        // block device operations
        .read  = Terminal_provided_block_device_read,
        .prog  = Terminal_provided_block_device_prog,
        .erase = Terminal_provided_block_device_erase,
        .sync  = Terminal_provided_block_device_sync,

        // block device configuration
        .read_size = 16,
        .prog_size = 16,
        .block_size = TERMINAL_LFS_BUFF_PAGESIZE,
        .block_count = TERMINAL_LFS_BUFF_COUNT,
        .block_cycles = 100,
        .cache_size = 16,
        .lookahead_size = 16,
        .read_buffer=NULL,
        .prog_buffer=NULL,
        .lookahead_buffer=NULL,
        .name_max=0,
        .file_max=0,
        .attr_max=0
};

void Terminal_Lfs_Init() {
    // mount the filesystem
    int err = lfs_mount(&Terminal_lfs_roots, &Terminal_cfg);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&Terminal_lfs_roots, &Terminal_cfg);
        err = lfs_mount(&Terminal_lfs_roots, &Terminal_cfg);
    }

    Terminal_Mkdir("/");
    Terminal_Mkdir(TERMINAL_SYSTEM_ROOTS);
    Terminal_Mkdir(TERMINAL_SYSTEM_ROOTS "/home");
    Terminal_Mkdir(TERMINAL_SYSTEM_ROOTS "/bin");
    Terminal_Mkdir(TERMINAL_SYSTEM_ROOTS "/data");

    Terminal_ChangeDir(TERMINAL_SYSTEM_ROOTS);
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

        Terminal_ForeColor(enmCFC_Cyan);
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
        if (name[l] == '.' && IS_SLASH(name[l + 1])
            && (l == 0 || IS_SLASH(name[l - 1])))
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
        if (name[l] == '.' && name[l + 1] == '.' && IS_SLASH(name[l + 2])
            && (l == 0 || IS_SLASH(name[l - 1]))) {
            int m = l + 3, n;

            l = l - 2;
            if (l >= 0) {
                while (l >= 0 && !IS_SLASH(name[l]))
                    l--;
                l++;
            } else
                l = 0;
            n = l;
            while ((name[n] = name[m]))
                (++n, ++m);
        } else
            ++l;
    }

    /* d) remove trailing xx/.. segment. */
    if (l == 2 && name[0] == '.' && name[1] == '.')
        name[0] = '\0';
    else if (l > 2 && name[l - 1] == '.' && name[l - 2] == '.'
             && IS_SLASH(name[l - 3])) {
        l = l - 4;
        if (l >= 0) {
            while (l >= 0 && !IS_SLASH(name[l]))
                l--;
            l++;
        } else
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
    } else if (path[0] != '/') {
        len = strlen(terminalUserInfo.Path);
        memcpy(_path, terminalUserInfo.Path, len);
        if (_path[len - 1] != '/') {
            _path[len++] = '/';
        }
        memcpy(&_path[len], path, strlen(path));
        len += strlen(path);
    } else {
        memcpy(_path, TERMINAL_SYSTEM_ROOTS, strlen(TERMINAL_SYSTEM_ROOTS));
        memcpy(&_path[strlen(TERMINAL_SYSTEM_ROOTS)], path, strlen(path));
        len = strlen(TERMINAL_SYSTEM_ROOTS) + strlen(path);
    }
    _path[len] = 0;
    Terminal_ap_getparents(_path);
    len = strlen(_path);
    if (0 != strcmp(TERMINAL_SYSTEM_ROOTS "/", _path) && _path[len - 1] == '/') {
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
    const char *p = strstr(path, TERMINAL_SYSTEM_ROOTS);
    if (p) {
        if (path - p == 0) {
            return &path[strlen(TERMINAL_SYSTEM_ROOTS)];
        }
    }
    return path;
}

int Terminal_Mkdir(const char *path) {
    struct lfs_info _lfs_info;
    int err = lfs_stat(&Terminal_lfs_roots, path, &_lfs_info);
    if (err) {
        err = lfs_mkdir(&Terminal_lfs_roots, path);
    }
    return err;
}

void Terminal_init(Terminal_getc getc_cb, Terminal_putc putc_cb) {
    Terminal_getc_cb = getc_cb;
    Terminal_putc_cb = putc_cb;
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return;
    }

    Terminal_offset = 0;
    memset(Terminal_command, 0x00, 126);
    Terminal_Clear();
    UtilChipInfo chipInfo;
    UtilGetChipInfo(&chipInfo);
    Terminal_Printf("boot ");
    Terminal_Printf(chipInfo.BuildDate);
    Terminal_Printf("\n\n");
    Terminal_ForeColor(enmCFC_Cyan);
    Terminal_Printf(TERMINAL_BOLDFONT "kOs starting " TERMINAL_DEFFONT " \n");
    Terminal_ResetColor();
    Terminal_PrintfChipInfo(&chipInfo);

    Terminal_Printf("starting Lfs file system.\n");
    Terminal_Lfs_Init();
    Terminal_Printf("Lfs file system finish .\n");

    Terminal_ForeColor(enmCFC_Cyan);
    Terminal_Printf(TERMINAL_BOLDFONT "kOs finish " TERMINAL_DEFFONT " \n");
    Terminal_ResetColor();
    Terminal_Printf("Starting Terminal ... \n\n");
}

void Terminal_Free() {
    lfs_unmount(&Terminal_lfs_roots);
}

void Terminal_PrintfChipInfo(UtilChipInfo *chipInfo) {
    char da[10] = {};
    Terminal_Printf("CPU    : STM32\n");
    Terminal_Printf("FLASH  : ");
    Terminal_Printf(itoa(chipInfo->FlashSize, da, 10));
    Terminal_Printf("k\n");
    Terminal_Printf("ChipID : ");
    for (int8_t i = 0; i < 12; i++) {
        itoa(chipInfo->ChipID[i], da, 16);
        if (chipInfo->ChipID[i] < 16) {
            Terminal_Printf("0");
        }
        Terminal_Printf(UtilUpperChar((char *) da));
    }
    Terminal_Printf("\n");
    Terminal_Printf("VER    : V");
    Terminal_Printf(itoa(chipInfo->VersionMain, da, 10));
    Terminal_Printf(".");
    Terminal_Printf(itoa(chipInfo->VersionSub1, da, 10));
    Terminal_Printf(".");
    Terminal_Printf(itoa(chipInfo->VersionSub2, da, 10));
    Terminal_Printf(" \n");
}

void Terminal_lsb_release() {
    UtilChipInfo chipInfo;
    UtilGetChipInfo(&chipInfo);
    Terminal_Printf("boot ");
    Terminal_Printf(chipInfo.BuildDate);
    Terminal_Printf("\n");
    Terminal_PrintfChipInfo(&chipInfo);

    Terminal_print_logo();
}

void Terminal_print_logo() {
    Terminal_Printf("Welcome to\n"
                    "   _     ___\n"
                    "  | | __/ _ \\ ___\n"
                    "  | |/ / | | / __|\n"
                    "  |   <| |_| \\__ \\\n"
                    "  |_|\\_\\\\___/|___/\n\n");
    Terminal_Printf("For further information check:\n"
                    " " TERMINAL_UNDERLINEFONT "https://kekxv.github.com/" TERMINAL_DEFFONT " \n");
}

void Terminal_Color(ConsoleForegroundColor foreColor, ConsoleBackGroundColor backColor) {
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return;
    }
    if (enmCFC_Default == foreColor && enmCBC_Default == backColor) {
        Terminal_Printf("\x1b[0m");
    } else {
        char foreColorStr[6] = {};
        char backColorStr[6] = {};
        itoa(foreColor, foreColorStr, 10);
        itoa(backColor, backColorStr, 10);
        if (enmCBC_Default == backColor) {
            // Terminal_Printf("\x1b[%d;%dm", backColor, foreColor);
            Terminal_Printf("\x1b[");
            Terminal_Printf(backColorStr);
            Terminal_putc_cb(';');
            Terminal_Printf(foreColorStr);
            Terminal_putc_cb('m');
        } else {
            // Terminal_Printf("\x1b[%d;%dm", foreColor, backColor);
            Terminal_Printf("\x1b[");
            Terminal_Printf(foreColorStr);
            Terminal_putc_cb(';');
            Terminal_Printf(backColorStr);
            Terminal_putc_cb('m');
        }
    }
}


void Terminal_PrintPs() {
    Terminal_ForeColor(enmCFC_Yellow);
    Terminal_Printf("[");
    Terminal_Printf(terminalUserInfo.UserName);
    Terminal_Printf("@");
    Terminal_Printf(TERMINAL_SYSTEM_NAME);
    Terminal_ForeColor(enmCFC_Purple);
    Terminal_Printf(" ");
    Terminal_Printf(Terminal_GetDirPath());
    Terminal_Printf(" ");
    Terminal_ForeColor(enmCFC_Yellow);
    Terminal_Printf("]# ");
    Terminal_ResetColor();
}

/**
 * 读取命令
 * @return
 */
int Terminal_ReadCommand() {
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return 0;
    }
    Terminal_offset = 0;
    char ch = 0;
    memset(Terminal_command, 0x00, TERMINAL_COMMAND_MAX_LENGTH);
    do {
        ch = Terminal_getc_cb();
        if (ch == 0) {
            continue;
        }
        if (ch == '\r' || ch == '\n') {
            Terminal_Printf("\r\n");
            break;
        }
        switch (ch) {
            case 0x1B://ESC
                ch = Terminal_getc_cb();
                // VT 模式 [ 开启
                if (ch != '[') {
                    continue;
                } else {
                    uint8_t args[5] = {};
                    uint8_t offset = 0;
                    do {
                        ch = Terminal_getc_cb();
                        args[offset++] = ch;
                        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '~') {
                            break;
                        }
                    } while (offset < 5);
                    // 参数错误
                    if (offset == 0 || offset == 5) {
                        continue;
                    }
                    Terminal_VTMode(args, offset);
                }
                continue;
                break;
            case 0x08://删除
            case 0x06:
            case 0x07:
            case 0x7E:
            case 0x7F:
                if (Terminal_offset <= 0)break;
                Terminal_offset--;
                //首先左移一下游标,然后记录下位置
                Terminal_putc_cb(0x08);
                Terminal_Printf(TERMINAL_SAVECURSOR);
                //在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
                Terminal_Printf(&Terminal_command[Terminal_offset + 1]);
                Terminal_Printf(" " TERMINAL_RESTORECURSOR);//恢复光标位置
                //在内存里把光标后面的内容全部往前移一格(包括末尾的\0)
                for (uint8_t i = Terminal_offset; i < (uint8_t) strlen(Terminal_command); i++) {
                    Terminal_command[i] = Terminal_command[i + 1];
                }
                continue;
                break;
            case '\t':
                break;
            default: {
                int16_t temp = strlen(Terminal_command);
                //把当前位置以及之后的内容全部后移,然后在当前位置的字符填入用户的按键
                for (int16_t i = temp - 1; i >= Terminal_offset; i--) {
                    Terminal_command[i + 1] = Terminal_command[i];
                }
                Terminal_putc_cb(ch);
                Terminal_Printf(TERMINAL_SAVECURSOR);
                //在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
                Terminal_Printf(&Terminal_command[Terminal_offset + 1]);
                Terminal_Printf(" " TERMINAL_RESTORECURSOR);//恢复光标位置
                Terminal_command[Terminal_offset++] = ch;
            }
                break;
        }
    } while (Terminal_offset < TERMINAL_COMMAND_MAX_LENGTH);
    if (Terminal_offset > 0) {
        UtilTrimSpace(Terminal_command);
    }
    return Terminal_offset;
}

void Terminal_VTMode(const uint8_t *args, uint8_t offset) {
    switch (args[offset - 1]) {
        case 'D'://光标左移
            if (Terminal_offset > 0) {
                Terminal_offset--;
                Terminal_putc_cb(0x08);
            }
            break;
        case 'C'://光标右移
            if (Terminal_command[Terminal_offset] != '\0') {
                Terminal_offset++;
                Terminal_Printf("\033[C");
            }
            break;

        case '~'://按键信息
            if (offset == 2) {
                char Terminal_offsetStr[6] = {};
                itoa(Terminal_offset, Terminal_offsetStr, 10);
                switch (args[0]) {
                    case 1://Home
                        Terminal_Printf("\033[");
                        Terminal_Printf(Terminal_offsetStr);
                        Terminal_putc_cb('D');
                        Terminal_offset = 0;
                        break;
                    case 4://End
                        if (Terminal_command[Terminal_offset] == 0)break;
                        Terminal_Printf("\033[");
                        memset(Terminal_offsetStr, 0x00, sizeof(Terminal_offsetStr));
                        itoa((int) strlen(Terminal_command) - Terminal_offset, Terminal_offsetStr, 10);
                        Terminal_Printf(Terminal_offsetStr);
                        Terminal_putc_cb('C');
                        Terminal_offset = strlen(Terminal_command);
                        break;
                    case 3://Delete
                        if (Terminal_command[Terminal_offset] == 0)break;
                        //记录下游标位置,然后内容前移,恢复游标位置
                        Terminal_Printf(TERMINAL_SAVECURSOR);
                        //在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
                        Terminal_Printf(&Terminal_command[Terminal_offset + 1]);
                        Terminal_putc_cb(0x20);//在末尾输出个空格,覆盖末尾的字留下的残影.
                        Terminal_Printf(" " TERMINAL_RESTORECURSOR);//恢复光标位置
                        //在内存里把光标后面的内容全部往前移一格(包括末尾的\0)
                        for (uint8_t i = Terminal_offset; i < (uint8_t) strlen(Terminal_command); i++) {
                            Terminal_command[i] = Terminal_command[i + 1];
                        }
                        break;
                }
            }
            break;
        default:
            break;
    }
}

/**
 * 获取命令内容
 * @return
 */
char *Terminal_GetCommand() {
    return Terminal_command;
}

void Terminal_Printf(const char *str) {
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return;
    }
    uint8_t offset = 0;
    while (str[offset] != '\0' && offset < 0xFe) {
        if (str[offset] == '\n') {
            Terminal_putc_cb('\r');
        }
        Terminal_putc_cb(str[offset++]);
    }
}

int Terminal_Run() {
    if (Terminal_Login() != 0) {
        return 0;
    }
    Terminal_Printf("Last login on tty \n");
    int reboot_flag = 0;
    while (1) {
        Terminal_PrintPs();
        if (Terminal_ReadCommand() > 0) {
            if (strcmp(Terminal_GetCommand(), "exit") == 0) {
                Terminal_Clear();
                break;
            } else if (strcmp(Terminal_GetCommand(), "reboot") == 0) {
                reboot_flag = 1;
                break;
            } else if (0 == strcmp(Terminal_GetCommand(), "pwd")) {
                Terminal_Printf(Terminal_GetDirPath());
                Terminal_Printf("\n");
            } else if (0 == strcmp(Terminal_GetCommand(), "lsb_release")) {
                Terminal_lsb_release();
            } else if (0 == strcmp(Terminal_GetCommand(), "clear")) {
                Terminal_Clear();
            } else {
                TerminalCommand_Run();
            }
        }
    }
    Terminal_Logout();
    return reboot_flag;
}

uint8_t Terminal_Login() {
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return 1;
    }
    memset(&terminalUserInfo, 0x00, sizeof(TerminalUserInfo));
#if ENABLE_SYSTEM_LOGIN
    Terminal_Printf(TERMINAL_SYSTEM_NAME " login: ");
    if (Terminal_ReadCommand() == 0) {
        return 2;
    }
    char password[20] = {};
    char ch = 0;
    uint8_t passwordOffset = 0;
    Terminal_Printf("Password:");
    do {
        ch = Terminal_getc_cb();
        if (ch == 0) {
            continue;
        }
        if (ch == '\r' || ch == '\n') {
            Terminal_Printf("\r\n");
            break;
        }
        Terminal_Printf("*");
        password[passwordOffset++] = ch;
    } while (1);
    if (strcmp(TERMINAL_USER, Terminal_GetCommand()) != 0
        || strcmp(TERMINAL_PASSWORD, password) != 0) {
        Terminal_Printf("Permission denied.\n");
        return 2;
    }
    memcpy(terminalUserInfo.UserName, Terminal_GetCommand(), strlen(Terminal_GetCommand()));
#else
    memcpy(terminalUserInfo.UserName, TERMINAL_USER, strlen(TERMINAL_USER));
#endif
    Terminal_print_logo();
    memset(terminalUserInfo.History, 0x00, sizeof(terminalUserInfo.History));
    if (0 == strcmp("root", terminalUserInfo.UserName)) {
        Terminal_ChangeDirSystemRoots("/");
    } else {
        Terminal_ChangeDirSystemRoots("/home");
        Terminal_Mkdir(terminalUserInfo.UserName);
        Terminal_ChangeDirSystemRoots(terminalUserInfo.UserName);
    }
    return 0;
}

void Terminal_Logout() {
    memset(&terminalUserInfo, 0x00, sizeof(TerminalUserInfo));
}


uint8_t Terminal_file_Buff[TERMINAL_LFS_BUFF_COUNT][TERMINAL_LFS_BUFF_PAGESIZE];

int Terminal_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
                                        lfs_off_t off, void *buffer, lfs_size_t size) {
    memcpy(buffer, &Terminal_file_Buff[block][off], size);
    return 0;
}

int Terminal_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
                                        lfs_off_t off, const void *buffer, lfs_size_t size) {
    memcpy(&Terminal_file_Buff[block][off], buffer, size);
    return 0;
}

int Terminal_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block) {
    memset(Terminal_file_Buff[block], 0x00, sizeof(Terminal_file_Buff[block]));
    return 0;
}

int Terminal_provided_block_device_sync(const struct lfs_config *c) {
    return 0;
}

