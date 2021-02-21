//
// Created by caesar kekxv on 2020/9/28.
//

#ifndef KOS_XMODEM_H
#define KOS_XMODEM_H

#include "main.h"

#define SOH     0x01	//Xmodem数据头
#define STX     0x02	//1K-Xmodem数据头
#define EOT     0x04	//发送结束
#define ACK     0x06	//认可响应
#define NAK     0x15	//不认可响应
#define CAN     0x18	//撤销传送
#define CTRLZ   0x1A	//填充数据包

int XMODEM_xmodem(int argc, const char *argv[]);

HAL_StatusTypeDef XMODEM_waitByte(uint8_t ch);
char XMODEM_readByte();


#endif //KOS_XMODEM_H
