//
// Created by caesar kekxv on 2020/9/15.
//

#include "terminal_command.h"
#include "stdlib.h"
#include "string.h"
#include "terminal.h"
#include "terminal_filesystem.h"

extern TerminalUserInfo terminalUserInfo;

void TerminalCommand_Run() {
    char *command = Terminal_GetCommand();
    size_t len = strlen(command);
    char *p = strchr(command, ' ');
    size_t offset = 0;
    char back = ' ';
    if (p != NULL) {
        offset = p - command;
        back = command[offset];
        command[offset] = 0;
    }

    if (0 == strcmp(command, "GPIO")) {
        if (offset == 0) {
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                offset = p - command;
                command[offset] = 0;
                TerminalCommand_Gpio(command, &command[offset + 1]);
            }
            else {
                TerminalCommand_Gpio(command, NULL);
            }
        }
    }
    else if (0 == strcmp(command, "cd")) {
        if (offset != 0) {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("cd : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                Terminal_ChangeDirSystemRoots(command);
            }
        }
    }
    else if (0 == strcmp(command, "mkdir")) {
        if (offset == 0) {
            Terminal_Printf("mkdir : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("mkdir : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                int err = Terminal_Mkdir(command);
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
    }
    else if (0 == strcmp(command, "rm")) {
        if (offset == 0) {
            Terminal_Printf("rm : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("rm : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                if (0 != Terminal_Remove(command)) {
                    Terminal_Printf("rm : ");
                    Terminal_ForeColor(enmCFC_Red);
                    Terminal_Printf("No such file or directory ");
                    Terminal_ResetColor();
                    Terminal_Printf("\n");
                }
            }
        }
    }
    else if (0 == strcmp(command, "mv")) {
        if (offset == 0) {
            Terminal_Printf("mv : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                *p = 0;
                if (0 != Terminal_Rename(command, p + 1)) {
                    Terminal_Printf("mv : ");
                    Terminal_ForeColor(enmCFC_Red);
                    Terminal_Printf("No such file or directory ");
                    Terminal_ResetColor();
                    Terminal_Printf("\n");
                }
            }
            else {
                Terminal_Printf("mv : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
        }
    }
    else if (0 == strcmp(command, "ls")) {
        if (offset == 0) {
            Terminal_ListsDirSystemRoots("");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("ls : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                if (0 != Terminal_ListsDirSystemRoots(command)) {
                    Terminal_Printf("ls : not found dir : ");
                    Terminal_ForeColor(enmCFC_Red);
                    Terminal_Printf(command);
                    Terminal_ResetColor();
                    Terminal_Printf("\n");
                }
            }
        }
    }
    else if (0 == strcmp(command, "df")) {
        if (0 < Terminal_FileSystemSize()) {
            Terminal_Printf("df : not found dir : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf(command);
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
    }
    else if (0 == strcmp(command, "touch")) {
        if (offset == 0) {
            Terminal_Printf("touch : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("touch : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                int err = Terminal_CreateFile(command);
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
    }
    else if (0 == strcmp(command, "cat")) {
        if (offset == 0) {
            Terminal_Printf("cat : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("cat : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                int err = Terminal_CatFile(command);
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
    }
    else if (0 == strcmp(command, "rz")) {
        if (0 < Terminal_ZmodemRz()) {
            Terminal_Printf("rz : rz command error: ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf(command);
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
    }
    else if (0 == strcmp(command, "sz")) {
        if (offset == 0) {
            Terminal_Printf("sz : ");
            Terminal_ForeColor(enmCFC_Red);
            Terminal_Printf("bad param ");
            Terminal_ResetColor();
            Terminal_Printf("\n");
        }
        else {
            memcpy(command, &command[offset + 1], len - offset);
            command[len - offset] = 0;
            UtilTrimSpace(command);
            p = strchr(command, ' ');
            if (p != NULL) {
                Terminal_Printf("sz : ");
                Terminal_ForeColor(enmCFC_Red);
                Terminal_Printf("bad param ");
                Terminal_ResetColor();
                Terminal_Printf("\n");
            }
            else {
                int err = Terminal_CatFile(command);
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
    }
    else {
        if (offset > 0) {
            command[offset] = back;
        }
        Terminal_Printf("command not found:");
        Terminal_ForeColor(enmCFC_Blue);
        Terminal_Printf(Terminal_GetCommand());
        Terminal_Printf("\n");
        Terminal_ResetColor();
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

