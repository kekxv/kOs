//
// Created by caesar kekxv on 2020/9/14.
//

#include "terminal.h"
#include "main.h"

#if defined(ENABLE_SSD1306_SPI) || defined(ENABLE_OLED_SSD1306_SOFT_SPI)
#include "ssd1306.h"
#include <stdbool.h>
#endif

#include "string.h"
#include "terminal_command.h"
#include "terminal_filesystem.h"
#include "util.h"
#include <stdlib.h>

TerminalUserInfo terminalUserInfo = {};

int8_t Terminal_offset = 0;
char Terminal_command[TERMINAL_COMMAND_MAX_LENGTH] = {};

Terminal_getc Terminal_getc_cb = NULL;
Terminal_putc Terminal_putc_cb = NULL;

void Terminal_init(Terminal_getc getc_cb, Terminal_putc putc_cb) {
    Terminal_getc_cb = getc_cb;
    Terminal_putc_cb = putc_cb;
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return;
    }

#ifdef ENABLE_OLED_SSD1306_SOFT_SPI
    ssd1306_Init();
    ssd1306_WriteString(0, 0 * 16, "kOs.kekxv.com", TYPE16X16, TYPE8X16);
#endif
#ifdef ENABLE_SSD1306_SPI
    ssd1306_Init();
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("kOs.kekxv.com", Font_6x8, White);
    ssd1306_UpdateScreen();
#endif
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
#ifdef ENABLE_OLED_SSD1306_SOFT_SPI
    LCD_Print(0, 1 * 16, "Lfs Finish", TYPE16X16, TYPE8X16);
#endif
#ifdef ENABLE_SSD1306_SPI
    ssd1306_SetCursor(0, 1 * 8);
    ssd1306_WriteString("Lfs & Terminal Finish", Font_6x8, White);
    ssd1306_UpdateScreen();
#endif

    Terminal_ForeColor(enmCFC_Cyan);
    Terminal_Printf(TERMINAL_BOLDFONT "kOs finish " TERMINAL_DEFFONT " \n");
    Terminal_ResetColor();
    Terminal_Printf("Starting Terminal ... \n\n");
#ifdef ENABLE_OLED_SSD1306_SOFT_SPI
    LCD_Print(0, 2 * 16, "Terminal Finish", TYPE16X16, TYPE8X16);
    LCD_Print(0, 3 * 16, "kOs boot Finish", TYPE16X16, TYPE8X16);
#endif
#ifdef ENABLE_SSD1306_SPI
    ssd1306_SetCursor(0, 2 * 8);
    ssd1306_WriteString("Welcome to", Font_6x8, White);
    ssd1306_SetCursor(0, 3 * 8);
    ssd1306_WriteString("   _     ___", Font_6x8, White);
    ssd1306_SetCursor(0, 4 * 8);
    ssd1306_WriteString("  | | __/ _ \\ ___", Font_6x8, White);
    ssd1306_SetCursor(0, 5 * 8);
    ssd1306_WriteString("  | |/ / | | / __|", Font_6x8, White);
    ssd1306_SetCursor(0, 6 * 8);
    ssd1306_WriteString("  |   <| |_| \\__ \\", Font_6x8, White);
    ssd1306_SetCursor(0, 7 * 8);
    ssd1306_WriteString("  |_|\\_\\\\___/|___/", Font_6x8, White);
    ssd1306_UpdateScreen();
#endif
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
    }
    else {
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
        }
        else {
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
    char ch;
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
            case 0x1B: //ESC
                ch = Terminal_getc_cb();
                // VT 模式 [ 开启
                if (ch != '[') {
                    continue;
                }
                else {
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
                // break;
            case 0x08: //删除
            case 0x06:
            case 0x07:
            case 0x7E:
            case 0x7F:
                if (Terminal_offset <= 0) break;
                Terminal_offset--;
                //首先左移一下游标,然后记录下位置
                Terminal_putc_cb(0x08);
                Terminal_Printf(TERMINAL_SAVECURSOR);
                //在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
                Terminal_Printf(&Terminal_command[Terminal_offset + 1]);
                Terminal_Printf(" " TERMINAL_RESTORECURSOR); //恢复光标位置
                //在内存里把光标后面的内容全部往前移一格(包括末尾的\0)
                for (uint8_t i = Terminal_offset; i < (uint8_t) strlen(Terminal_command); i++) {
                    Terminal_command[i] = Terminal_command[i + 1];
                }
                continue;
                // break;
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
                Terminal_Printf(" " TERMINAL_RESTORECURSOR); //恢复光标位置
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
        case 'D': //光标左移
            if (Terminal_offset > 0) {
                Terminal_offset--;
                Terminal_putc_cb(0x08);
            }
            break;
        case 'C': //光标右移
            if (Terminal_command[Terminal_offset] != '\0') {
                Terminal_offset++;
                Terminal_Printf("\033[C");
            }
            break;

        case '~': //按键信息
            if (offset == 2) {
                char Terminal_offsetStr[6] = {};
                itoa(Terminal_offset, Terminal_offsetStr, 10);
                switch (args[0]) {
                    case 1: //Home
                        Terminal_Printf("\033[");
                        Terminal_Printf(Terminal_offsetStr);
                        Terminal_putc_cb('D');
                        Terminal_offset = 0;
                        break;
                    case 4: //End
                        if (Terminal_command[Terminal_offset] == 0) break;
                        Terminal_Printf("\033[");
                        memset(Terminal_offsetStr, 0x00, sizeof(Terminal_offsetStr));
                        itoa((int) strlen(Terminal_command) - Terminal_offset, Terminal_offsetStr, 10);
                        Terminal_Printf(Terminal_offsetStr);
                        Terminal_putc_cb('C');
                        Terminal_offset = strlen(Terminal_command);
                        break;
                    case 3: //Delete
                        if (Terminal_command[Terminal_offset] == 0) break;
                        //记录下游标位置,然后内容前移,恢复游标位置
                        Terminal_Printf(TERMINAL_SAVECURSOR);
                        //在内存里输出光标后的内容,此时会覆盖光标里的字,相当于原地删除了
                        Terminal_Printf(&Terminal_command[Terminal_offset + 1]);
                        Terminal_putc_cb(0x20);                      //在末尾输出个空格,覆盖末尾的字留下的残影.
                        Terminal_Printf(" " TERMINAL_RESTORECURSOR); //恢复光标位置
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

void Terminal_PrintfNum(int32_t str) {
    char da[6];
    Terminal_Printf(itoa(str, da, 10));
}

void Terminal_PrintfChar(char ch) {
    if (Terminal_getc_cb == NULL ||
        Terminal_putc_cb == NULL) {
        return;
    }
    Terminal_putc_cb(ch);
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
            }
            else if (strcmp(Terminal_GetCommand(), "reboot") == 0) {
                reboot_flag = 1;
                break;
            }
            else if (0 == strcmp(Terminal_GetCommand(), "pwd")) {
                Terminal_Printf(Terminal_GetDirPath());
                Terminal_Printf("\n");
            }
            else if (0 == strcmp(Terminal_GetCommand(), "lsb_release")) {
                Terminal_lsb_release();
            }
            else if (0 == strcmp(Terminal_GetCommand(), "clear")) {
                Terminal_Clear();
            }
            else {
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
    char ch; // = 0;
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
    if (strcmp(TERMINAL_USER, Terminal_GetCommand()) != 0 || strcmp(TERMINAL_PASSWORD, password) != 0) {
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
    }
    else {
        Terminal_ChangeDirSystemRoots("/home");
        Terminal_Mkdir(terminalUserInfo.UserName);
        Terminal_ChangeDirSystemRoots(terminalUserInfo.UserName);
    }
    return 0;
}

void Terminal_Logout() {
    memset(&terminalUserInfo, 0x00, sizeof(TerminalUserInfo));
}
