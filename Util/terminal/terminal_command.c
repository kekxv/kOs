//
// Created by caesar kekxv on 2020/9/15.
//

#include "terminal_command.h"
#include "stdlib.h"
#include "string.h"
#include "terminal.h"
#include "terminal_filesystem.h"

extern TerminalUserInfo terminalUserInfo;


void TerminalCommand_Run(int argc, char **argv) {
    char *command = argv[0];
    if (0 == strcmp(command, "GPIO")) {
        if (argc == 3) {
            TerminalCommand_Gpio(argv[1], argv[2]);
        }
        else if (argc == 2) {
            TerminalCommand_Gpio(argv[1], NULL);
        }
    }
    else if (0 == strcmp(command, "cd")) {
        if (argc == 2) {
            Terminal_ChangeDirSystemRoots(argv[1]);
        }
        else {
            Terminal_Printf("cd : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
    }
    else if (0 == strcmp(command, "mkdir")) {
        if (argc != 2) {
            Terminal_Printf("mkdir : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            int err = Terminal_Mkdir(argv[1]);
            if (0 != err) {
                Terminal_Printf("mkdir : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("can't create dir : ");
                Terminal_PrintfNum(err);
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "rm")) {
        if (argc != 2) {
            Terminal_Printf("rm : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            if (0 != Terminal_Remove(argv[1])) {
                Terminal_Printf("rm : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("No such file or directory ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "mv")) {
        if (argc != 3) {
            Terminal_Printf("mv : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            if (0 != Terminal_Rename(argv[1], argv[2])) {
                Terminal_Printf("mv : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("No such file or directory ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "ls")) {
        if (argc != 2) {
            Terminal_ListsDirSystemRoots("");
        }
        else {
            if (0 != Terminal_ListsDirSystemRoots(argv[1])) {
                Terminal_Printf("ls : not found dir : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf(argv[1]);
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "df")) {
        if (0 < Terminal_FileSystemSize()) {
            Terminal_Printf("df : command error \n");
        }
    }
    else if (0 == strcmp(command, "touch")) {
        if (argc != 2) {
            Terminal_Printf("touch : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            int err = Terminal_CreateFile(argv[1]);
            if (0 != err) {
                Terminal_Printf("touch : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("can't create file : ");
                Terminal_PrintfNum(err);
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "cat")) {
        if (argc != 2) {
            Terminal_Printf("cat : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            int err = Terminal_CatFile(argv[1]);
            if (0 != err) {
                Terminal_Printf("cat : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("No such file or Is a directory : ");
                Terminal_PrintfNum(err);
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "rz")) {
        if (0 < Terminal_ZmodemRz()) {
            Terminal_Printf("rz : rz command error \n");
        }
    }
    else if (0 == strcmp(command, "sz")) {
        if (argc != 2) {
            Terminal_Printf("sz : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            int err = Terminal_CatFile(argv[1]);
            if (0 != err) {
                Terminal_Printf("sz : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("sz : sz command error : ");
                Terminal_PrintfNum(err);
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else {
        Terminal_Printf("command not found:");
        Terminal_ForeColor(enmCFC_Blue);
        Terminal_Printf(Terminal_GetCommand());
        Terminal_Printf("\n");
        Terminal_ResetColor();
    }
}

/**
 * 解析参数存储
 */
char Terminal_ParseArgsLine[TERMINAL_COMMAND_MAX_LENGTH + 1] = {};

/**
 * 解析参数
 * @param line
 * @param argc
 * @param argv
 */
void TerminalCommand_ParseArgs(const char *line, int *argc, char *argv[]) {
    *argc = 0;
    int len = strlen(line);
    memcpy((uint8_t *) Terminal_ParseArgsLine, (uint8_t *) line, len);
    Terminal_ParseArgsLine[len] = 0;
    for (int i = 0; i < len; i++) {
        while (Terminal_ParseArgsLine[i] == ' ') {
            Terminal_ParseArgsLine[i] = 0;
            i++;
        }
        if (Terminal_ParseArgsLine[i] == 0)break;
        argv[(int) (*argc)] = &Terminal_ParseArgsLine[i];
        (*argc)++;
        while (i < len) {
            i++;
            if (Terminal_ParseArgsLine[i] == ' ') {
                Terminal_ParseArgsLine[i] = 0;
                break;
            }
            if (Terminal_ParseArgsLine[i] == 0) {
                break;
            }
        }
    }
}

void TerminalCommand_Gpio(char *Gpio, const char *state) {
    if (Gpio == NULL) return;
    char f = Gpio[0];
    int pIndex = strtol(&Gpio[1], NULL, 10);
    int s = -1;
    if (state == NULL) {
        s = UtilGpioRead(f, (uint8_t) pIndex);
    }
    else {
        s = UtilGpio(f, (uint8_t) pIndex, state[0] == '0' ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    Terminal_Printf("GPIO ");
    if (s == -1) {
        Terminal_Printf("bad param \n");
        return;
    }
    Terminal_Printf(Gpio);
    Terminal_Printf(" ");
    Terminal_Printf(state == NULL ? "Get" : "Set");
    Terminal_Printf(":");
    Terminal_ForeColor(enmCFC_Blue);
    Terminal_Printf(s == GPIO_PIN_RESET ? "GPIO_PIN_RESET" : "GPIO_PIN_SET");
    Terminal_ResetColor();
    Terminal_Printf("\n");
}

