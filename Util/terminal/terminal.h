//
// Created by caesar kekxv on 2020/9/14.
//

#ifndef KOS_TERMINAL_H
#define KOS_TERMINAL_H

#include "lfs.h"
#include "lfs_util.h"
#include "main.h"
#include "util.h"


#ifndef ENABLE_SYSTEM_LOGIN
#define ENABLE_SYSTEM_LOGIN 1
#endif // ENABLE_SYSTEM_LOGIN
#ifndef TERMINAL_SYSTEM_NAME
#define TERMINAL_SYSTEM_NAME "kOs"
#endif

#ifndef MAX_PATH_SIZE
#define MAX_PATH_SIZE 56
#endif
// 使用 FLASH 块大小作为存储
#ifndef TERMINAL_LFS_BUFF_COUNT
#define TERMINAL_LFS_BUFF_COUNT 4
#endif

#ifndef TERMINAL_USER
#define TERMINAL_USER "root"
#endif
#ifndef TERMINAL_PASSWORD
#define TERMINAL_PASSWORD "root"
#endif
#ifndef TERMINAL_COMMAND_MAX_LENGTH
#define TERMINAL_COMMAND_MAX_LENGTH 126
#endif

// Tips 开启 SSD 输出
// #ifndef ENABLE_SSD1306_SPI
// #define ENABLE_SSD1306_SPI
// #endif

#ifndef TERMINAL_VT_DEFIND
#define TERMINAL_VT_DEFIND
#define TERMINAL_DEFFONT "\033[0m"       /* 设置默认字体 */
#define TERMINAL_BOLDFONT "\033[1m"      /* 设置blod字体 */
#define TERMINAL_UNDERLINEFONT "\033[4m" /* 设置下划线字体 */
#define TERMINAL_CLEARSCREEN "\033[2J"   /* 清除屏幕 */
#define TERMINAL_CURSORHOME "\033[H"     /* 将光标还原到原点 */
#define TERMINAL_SAVECURSOR "\033[s"     /* 保存光标位置 */
#define TERMINAL_RESTORECURSOR "\033[u"  /* 将光标还原到保存位置 */
#define TERMINAL_SINGLEWIDTH "\033#5"    /* 普通、单宽字符 */
#define TERMINAL_DOUBLEWIDTH "\033#6"    /* 创建双宽度字符 */

#define TERMINAL_FREEZETOP "\033[2;25r"    /* 冻结顶部线 */
#define TERMINAL_FREEZEBOTTOM "\033[1;24r" /* 冻结底线 */
#define TERMINAL_UNFREEZE_TB "\033[r"      /* 解冻顶行和底线 */

#define TERMINAL_BLINKTEXT "\033[5m"          /* 闪烁文本 */
#define TERMINAL_REVERSEMODE "\033[7m"        /* 将终端设置为反向模式 */
#define TERMINAL_LIGHTREVERSEMODE "\033[1,7m" /* 将终端设置为灯光反转模式 */
#endif

#ifndef ZMODEM_RZ_STRING
#define ZMODEM_RZ_STRING  "rz waiting to receive.**B0100\r\n"
#endif
#ifndef ZMODEM_SZ_STRING
#define ZMODEM_SZ_STRING  "**B00000000000000\r\n"
#endif


#ifndef Terminal_ForeColor
/**
 * 设置前景色
 */
#define Terminal_ForeColor(consoleForegroundColor) Terminal_Color(consoleForegroundColor, enmCBC_Default)
#endif
#ifndef Terminal_ResetColor
/**
 * 恢复颜色
 */
#define Terminal_ResetColor() Terminal_Color(enmCFC_Default, enmCBC_Default)
#endif
#ifndef Terminal_Clear
/**
 * 清理画面
 */
#define Terminal_Clear() Terminal_Printf(TERMINAL_CLEARSCREEN TERMINAL_CURSORHOME)
#endif

typedef enum {
    enmCFC_Red = 31
    , enmCFC_Green = 32
    , enmCFC_Blue = 34
    , enmCFC_Yellow = 33
    , enmCFC_Purple = 35
    , enmCFC_Cyan = 36
    , enmCFC_White = 37
    , enmCFC_Black = 30
    , enmCFC_Default = 0
    , } ConsoleForegroundColor;

typedef enum {
    enmCBC_Red = 41
    , enmCBC_Green = 42
    , enmCBC_Blue = 44
    , enmCBC_Yellow = 43
    , enmCBC_Purple = 45
    , enmCBC_Cyan = 46
    , enmCBC_White = 47
    , enmCBC_Black = 40
    , enmCBC_Default = 0
    , } ConsoleBackGroundColor;


/**
 * 读取一个字符
 */
typedef HAL_StatusTypeDef (*Terminal_getc_timeout)(uint8_t *ch, int timeout);

typedef void (*Terminal_putc_timeout)(char ch, int timeout);

typedef struct {
    char UserName[15];
    char Path[50];
    char History[5][25];
} TerminalUserInfo;


/**
 * 初始化
 */
void Terminal_init(Terminal_getc_timeout getc_cb, Terminal_putc_timeout putc_cb);

void Terminal_Free();

/**
 * 设置颜色
 * @param consoleForegroundColor
 * @param consoleBackGroundColor
 */
void Terminal_Color(ConsoleForegroundColor consoleForegroundColor, ConsoleBackGroundColor consoleBackGroundColor);

/**
 * 读取命令
 * @return
 */
int Terminal_ReadCommand();

void Terminal_VTMode(const uint8_t *args, uint8_t offset);

void Terminal_PrintPs();

/**
 * 获取命令内容
 * @return
 */
char *Terminal_GetCommand();

void Terminal_Printf(const char *str);

void Terminal_PrintfNum(int32_t str);

void Terminal_PrintfChar(char ch);

/**
 * 开启终端模式
 */
int Terminal_Run();

uint8_t Terminal_Login();

void Terminal_Logout();

void Terminal_PrintfChipInfo(UtilChipInfo *chipInfo);

void Terminal_lsb_release();

void Terminal_print_logo();

/**
 * 上传到服务器
 * @return
 */
int Terminal_ZmodemRz();

/**
 * 从服务器下载
 * @param path
 * @return
 */
int Terminal_ZmodemSz(char *path);


#endif //KOS_TERMINAL_H
