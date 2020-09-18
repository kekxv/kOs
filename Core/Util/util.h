//
// Created by caesar kekxv on 2020/9/13.
//


#ifndef STM32F103C8T6_UTIL_H
#define STM32F103C8T6_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#ifndef __Util_STDPERIPH_VERSION_MAIN
#define __Util_STDPERIPH_VERSION_MAIN 0
#endif
#ifndef __Util_STDPERIPH_VERSION_SUB1
#define __Util_STDPERIPH_VERSION_SUB1 0
#endif
#ifndef __Util_STDPERIPH_VERSION_SUB2
#define __Util_STDPERIPH_VERSION_SUB2 1
#endif

#ifdef HAL_SPI_MODULE_ENABLED

#ifndef UtilSpiSendWait
#define UtilSpiSendWait(data, dataLen) UtilSpiSend(data, dataLen, 0xffff)
#endif // UtilSpiSendWait
#ifndef UtilSpiReadWait
#define UtilSpiReadWait(data, dataLen) UtilSpiRead(data, dataLen, 0xffff)
#endif // UtilSpiReadWait

HAL_StatusTypeDef UtilSpiSend(uint8_t *data, uint16_t dataLen, uint32_t timeout);

HAL_StatusTypeDef UtilSpiRead(uint8_t *data, uint16_t dataLen, uint32_t timeout);

#endif //HAL_SPI_MODULE_ENABLED

#ifdef HAL_UART_MODULE_ENABLED

void UtilClearRead();

/**
 * 发送一个字节到串口
 * @param ch
 */
void UtilPutc(char ch);

/**
 * 发送字符串到串口
 * @param ch
 */
void UtilPuts(const char *str);

void UtilPutsLine(const char *str);

/**
 * 获取一个字节
 * @return
 */
char UtilGetc();

/**
 * 获取一个字符串
 * @param str 字符串储存
 * @param len 字符串长度
 * @return 获取的字符串长度
 */
int UtilGets(char str[], uint8_t len);


/**
 * 发送 串口 数据
 * @param data
 * @param len
 * @return
 */
HAL_StatusTypeDef UtilUartSend(uint8_t data[], uint16_t len);

/**
 * 发送 串口 字符串
 * @param data
 * @return
 */
HAL_StatusTypeDef UtilUartSendStr(char data[]);

#endif


#ifdef HAL_PCD_MODULE_ENABLED
#include <usbd_ioreq.h>
/**
 * 发送 USB 数据
 * @param data
 * @param len
 * @return
 */
USBD_StatusTypeDef UtilUsbSend(uint8_t data[], uint16_t len);

/**
 * 发送 USB 字符串
 * @param data
 * @return
 */
USBD_StatusTypeDef UtilUsbSendStr(char data[]);

#endif //HAL_PCD_MODULE_ENABLED

#ifdef HAL_GPIO_MODULE_ENABLED

#ifndef UtilGpioRead
#define UtilGpioRead(GpioIndex, pIndex) UtilGpio(GpioIndex, pIndex, -1)
#endif

typedef enum {
    GPIO_PIN_LOW = 0, //低电平
    GPIO_PIN_HIGH //高电平
}GPIO_PIN_STATE;

int16_t UtilGpio(char GpioIndex, uint8_t pIndex, int8_t PinState);

#endif //HAL_GPIO_MODULE_ENABLED

typedef struct {
    uint8_t ChipID[12];
    uint16_t FlashSize;
    char BuildDate[25];
    uint16_t VersionMain;
    uint16_t VersionSub1;
    uint16_t VersionSub2;
} UtilChipInfo;

/**
 * 读取芯片 ID
 * @param ChipID
 */
void UtilGetChipID(uint8_t ChipID[12]);

/**
 * 读取芯片信息
 * @param chipInfo
 */
void UtilGetChipInfo(UtilChipInfo *chipInfo);

/**
 * 字符转大写
 * @param ch
 * @return
 */
const char *UtilUpperChar(char *ch);

/**
 * 字符转小写
 * @param ch
 * @return
 */
const char *UtilLowerChar(char *ch);


#ifndef UtilTrimSpace
#define UtilTrimSpace(GpioIndex) UtilTrim(GpioIndex, ' ')
#endif

/**
 * 字符转小写
 * @param ch
 * @return
 */
const char *UtilTrim(char *ch, char t);

#ifdef __cplusplus
}
#endif
#endif //STM32F103C8T6_UTIL_H
