//
// Created by caesar kekxv on 2020/9/15.
//

#ifndef KOS_TERMINAL_EXEC_H
#define KOS_TERMINAL_EXEC_H

void TerminalCommand_Run(int argc, char *argv[]);

/**
 * 解析参数
 * @param line
 * @param argc
 * @param argv
 */
void TerminalCommand_ParseArgs(const char *line, int *argc, char *argv[]);

void TerminalCommand_Gpio(char *Gpio, const char *state);

#endif //KOS_TERMINAL_EXEC_H
