//
// Created by caesar kekxv on 2020/9/13.
//


#include "string.h"
#include "main.h"
#include "util.h"


#ifdef HAL_SPI_MODULE_ENABLED
extern SPI_HandleTypeDef hspi1;

HAL_StatusTypeDef UtilSpiSend(uint8_t *data, uint16_t dataLen, uint32_t timeout) {
    return HAL_SPI_Transmit(&hspi1, data, dataLen, timeout);
}

HAL_StatusTypeDef UtilSpiRead(uint8_t *data, uint16_t dataLen, uint32_t timeout) {
    return HAL_SPI_Receive(&hspi1, data, dataLen, timeout);
}


#endif //HAL_SPI_MODULE_ENABLED

#ifdef HAL_UART_MODULE_ENABLED

#include "stdio.h"
#include "stdarg.h"

extern UART_HandleTypeDef huart1;

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define PUTCHAR_PROTOTYPE_CALL(x) __io_putchar(x)
#define GETCHAR_PROTOTYPE int __io_getchar(FILE *f)
#define GETCHAR_PROTOTYPE_CALL() __io_getchar(NULL)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define PUTCHAR_PROTOTYPE_CALL(x) fputc(x,NULL)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#define GETCHAR_PROTOTYPE_CALL() fgetc(NULL)
#endif /* __GNUC__ */


PUTCHAR_PROTOTYPE {
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART2 and Loop until the end of transmission */
    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xFFFF);

    return ch;
}

GETCHAR_PROTOTYPE {
    uint8_t ch = 0;
    HAL_UART_Receive(&huart1, (uint8_t *) &ch, 1, 0xFFFF);

    if (ch == '\r') {
        PUTCHAR_PROTOTYPE_CALL('\r');
        ch = '\n';
    }

    return PUTCHAR_PROTOTYPE_CALL(ch);
}

int _read(int file, char *ptr, int len) {
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        // *ptr++ = GETCHAR_PROTOTYPE_CALL();
        *ptr = GETCHAR_PROTOTYPE_CALL();
        if (*ptr++ == '\n') break;
    }
    // return len;
    return ++DataIdx;
}

void UtilClearRead() {
    __HAL_UART_CLEAR_OREFLAG(&huart1);
}

/**
 * 发送一个字节到串口
 * @param ch
 */
void UtilPutc(char ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xffff);
}

/**
 * 发送字符串到串口
 * @param ch
 */
void UtilPuts(const char *str) {
    uint8_t offset = 0;
    while (str[offset] != '\0' && offset < 0xFe) {
        if (str[offset] == '\n') {
            UtilPutc('\r');
        }
        UtilPutc(str[offset++]);
    }
}

/**
 * 发送字符串到串口
 * @param ch
 */
void UtilPutsLine(const char *str) {
    UtilPuts(str);
    UtilPutc('\r');
    UtilPutc('\n');
}

/**
 * 获取一个字节
 * @return
 */
char UtilGetc() {
    uint8_t ch = 0;
    HAL_StatusTypeDef ret = HAL_UART_Receive(&huart1, (uint8_t *) &ch, 1, 0xffff);
    // if (ret == HAL_OK && ch != 0) {
    //     UtilPutc(ch);
    // }
    return ch;
}

/**
 * 获取一个字符串
 * @param str 字符串储存
 * @param len 字符串长度
 * @return 获取的字符串长度
 */
int UtilGets(char str[], uint8_t len) {
    uint8_t offset = 0;
    if (len == 0)return 0;
    len--;
    while (offset < len) {
        str[offset] = UtilGetc();
        if (str[offset] == '\0') {
            break;
        }
        if (str[offset] == '\r' || str[offset] == '\n') {
            str[offset++] = '\0';
            break;
        }
        offset++;
    }
    str[offset] = 0;
    return offset;
}

/**
 * 发送 串口 数据
 * @param data
 * @param len
 * @return
 */
HAL_StatusTypeDef UtilUartSend(uint8_t data[], uint16_t len) {
    return HAL_UART_Transmit(&huart1, data, len, 0xffff);
}

/**
 * 发送 串口 字符串
 * @param data
 * @return
 */
HAL_StatusTypeDef UtilUartSendStr(char data[]) {
    return UtilUartSend((uint8_t *) data, strlen(data));
}

#endif

#ifdef HAL_PCD_MODULE_ENABLED
extern USBD_HandleTypeDef hUsbDeviceFS;
// USBD_HandleTypeDef hUsbDeviceFS;
/**
 * 发送 USB 数据
 * @param data
 * @param len
 * @return
 */
USBD_StatusTypeDef UtilUsbSend(uint8_t data[], uint16_t len) {
    return USBD_CtlSendData(&hUsbDeviceFS, data, len);
}

/**
 * 发送 USB 字符串
 * @param data
 * @return
 */
USBD_StatusTypeDef UtilUsbSendStr(char data[]) {
    return UtilUsbSend((uint8_t *) data, strlen(data));
}
#endif


#ifdef HAL_GPIO_MODULE_ENABLED

int16_t UtilGpio(char GpioIndex, uint8_t pIndex, int8_t PinState) {
    if (pIndex > 15)return -1;
    GPIO_TypeDef *GPIOx = GPIOA;
    switch (GpioIndex) {
        case 'A':
            GPIOx = GPIOA;
            break;
        case 'B':
            GPIOx = GPIOB;
            break;
        case 'C':
            GPIOx = GPIOC;
            break;
        case 'D':
            GPIOx = GPIOD;
            break;
        case 'E':
            GPIOx = GPIOE;
            break;
        default:
            return -1;
    }
    if (PinState == 0 || PinState == 1) {
        HAL_GPIO_WritePin(GPIOx, (uint16_t) 0b1 << pIndex, PinState);
    }
    return HAL_GPIO_ReadPin(GPIOx, (uint16_t) 0b1 << pIndex);
}

#endif //HAL_GPIO_MODULE_ENABLED


#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

/**
 * 读取芯片 ID
 * @param ChipID
 */
void UtilGetChipID(uint8_t ChipID[12]) {
    uint32_t temp0, temp1, temp2;
    temp0 = *(__IO uint32_t *) (0x1FFFF7E8);    //产品唯一身份标识寄存器（96位）
    temp1 = *(__IO uint32_t *) (0x1FFFF7EC);
    temp2 = *(__IO uint32_t *) (0x1FFFF7F0);
    ChipID[0] = (uint8_t) (temp0 & 0x000000FF);
    ChipID[1] = (uint8_t) ((temp0 & 0x0000FF00) >> 8);
    ChipID[2] = (uint8_t) ((temp0 & 0x00FF0000) >> 16);
    ChipID[3] = (uint8_t) ((temp0 & 0xFF000000) >> 24);
    ChipID[4] = (uint8_t) (temp1 & 0x000000FF);
    ChipID[5] = (uint8_t) ((temp1 & 0x0000FF00) >> 8);
    ChipID[6] = (uint8_t) ((temp1 & 0x00FF0000) >> 16);
    ChipID[7] = (uint8_t) ((temp1 & 0xFF000000) >> 24);
    ChipID[8] = (uint8_t) (temp2 & 0x000000FF);
    ChipID[9] = (uint8_t) ((temp2 & 0x0000FF00) >> 8);
    ChipID[10] = (uint8_t) ((temp2 & 0x00FF0000) >> 16);
    ChipID[11] = (uint8_t) ((temp2 & 0xFF000000) >> 24);
}

#pragma clang diagnostic pop


void UtilGetChipInfo(UtilChipInfo *chipInfo) {
    memset(chipInfo, 0x00, sizeof(UtilChipInfo));
    chipInfo->FlashSize = *(uint16_t *) (0x1FFFF7E0);    //闪存容量寄存器
    UtilGetChipID(chipInfo->ChipID);
    const char *buildData = __DATE__ " " __TIME__;
    memcpy(chipInfo->BuildDate, buildData, strlen(buildData));
    chipInfo->VersionMain = __Util_STDPERIPH_VERSION_MAIN;
    chipInfo->VersionSub1 = __Util_STDPERIPH_VERSION_MAIN;
    chipInfo->VersionSub2 = __Util_STDPERIPH_VERSION_MAIN;

}

const char *UtilUpperChar(char *ch) {
    for (size_t i = 0; i < strlen(ch); i++) {
        if (ch[i] >= 'a' && ch[i] <= 'z') {
            ch[i] = (char) (ch[i] - ('a' - 'A'));
        }
    }
    return ch;
}

const char *UtilLowerChar(char *ch) {
    for (size_t i = 0; i < strlen(ch); i++) {
        if (ch[i] >= 'A' && ch[i] <= 'Z') {
            ch[i] = (char) (ch[i] + ('a' - 'A'));
        }
    }
    return ch;
}

/**
 * 字符转小写
 * @param ch
 * @return
 */
const char *UtilTrim(char *ch, char t) {
    if (ch == NULL) { return ch; }
    size_t len = strlen(ch);
    size_t bIndex = 0;
    size_t eIndex = 0;
    for (bIndex = 0; bIndex < len; bIndex++) {
        if (ch[bIndex] != t) {
            break;
        }
    }
    if (bIndex == len) {
        ch[0] = 0;
        return ch;
    }
    for (eIndex = len - 1; eIndex > bIndex; eIndex--) {
        if (ch[eIndex] != t) {
            break;
        }
    }
    if (eIndex < bIndex) {
        ch[0] = 0;
        return ch;
    }
    len = eIndex - bIndex + 1;
    memcpy(ch, &ch[bIndex], len);
    ch[len] = 0;
    return ch;
}
