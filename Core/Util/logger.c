//
// Created by caesar kekxv on 2020/9/13.
//


#include "logger.h"

void LoggerWrite(char level, const char *TAG, const char *message) {
    UtilPutc('[');
    UtilPutc(level);
    UtilPutc(']');
    UtilPutc('[');
    UtilPuts(TAG);
    UtilPutc(']');
    UtilPuts(message);
    UtilPutc('\n');
}
