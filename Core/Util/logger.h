//
// Created by caesar kekxv on 2020/9/13.
//

#ifndef STM32F103C8T6_LOGGER_H
#define STM32F103C8T6_LOGGER_H
#include "util.h"

#ifndef LoggerD
#define LoggerD(TAG, MESSAGE) LoggerWrite('D',TAG,MESSAGE)
#endif
#ifndef LoggerI
#define LoggerI(TAG, MESSAGE) LoggerWrite('I',TAG,MESSAGE)
#endif
#ifndef LoggerE
#define LoggerE(TAG, MESSAGE) LoggerWrite('E',TAG,MESSAGE)
#endif

void LoggerWrite(char level, const char *TAG, const char *message);


#endif //STM32F103C8T6_LOGGER_H
