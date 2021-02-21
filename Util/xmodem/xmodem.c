//
// Created by caesar kekxv on 2020/9/28.
//

#include "xmodem.h"
#include "terminal.h"

extern Terminal_getc_timeout Terminal_getc_timeout_cb;
extern Terminal_putc_timeout Terminal_putc_timeout_cb;

extern void Terminal_putc_cb(char);

HAL_StatusTypeDef XMODEM_waitByte(uint8_t ch) {
    int len = 100;
    HAL_StatusTypeDef ok;
    uint8_t c;
    do {
        ok = Terminal_getc_timeout_cb(&c, 0xffff);
        if (ok == HAL_OK && ch == c) {
            return ok;
        }
        len--;
    } while (len > 0);
    return HAL_TIMEOUT;
}

char XMODEM_readByte() {
    char ch;
    HAL_StatusTypeDef err = Terminal_getc_timeout_cb((uint8_t *) &ch, 0xFFFF);
    return ch;
}


int XMODEM_xmodem(int argc, const char *argv[]) {
    char run = 1;
    //起点地址
    long int addr = 0;
    //读写缓冲区
    char buff[0x400];
    long int i = 0;
    //包头
    char head = 0;
    //包号
    unsigned char packno = 0;
    //远程crc,本地crc
    unsigned char r_crc = 0, l_crc = 0;
    //包长
    int packlen = 128;
    HAL_StatusTypeDef err;
    uint8_t ch;
    switch (argc) {
        case 3:
            Terminal_Printf("please send file\n");
            err = XMODEM_waitByte('r');
            if (HAL_OK != err) {
                Terminal_Printf("read char r fail!");
                break;
            }
            err = Terminal_getc_timeout_cb(&ch, 0xff);
            if (ch != 'x') {
                Terminal_Printf("send fail!");
                break;
            }
            err = XMODEM_waitByte(0x0d);
            if (HAL_OK != err) {
                Terminal_Printf("read char 0x0d fail!");
                break;
            }
            bool flag = false;
            while (run) {
                do {
                    err = Terminal_getc_timeout_cb((uint8_t *) &head, 0x10);
                    //一直循环发送'C'直到客户端开始发文件
                    if (!flag && err == HAL_OK) {
                        flag = true;
                        break;
                    }
                    HAL_Delay(10);
                    Terminal_putc_cb(NAK);
                } while (true);
                switch (head) {
                    case CAN:
                        Terminal_Printf("中断传输!\n");
                        run = 0;
                        break;
                    case EOT:
                        Terminal_Printf("发送结束!\n");
                        run = 0;
                        break;
                    case SOH://xmodem
                    case STX://1k-xmodem
                        packlen = head == SOH ? 128 : 1024;
                        packno = XMODEM_readByte();
                        XMODEM_readByte();//包号反码,用于验证
                        l_crc = 0;
                        for (i = 0; i < packlen; i++) {
                            buff[i] = XMODEM_readByte();
                            l_crc += buff[i];//计算校验和
                        }
                        //l_crc=checksum8(buff,packlen);
                        r_crc = XMODEM_readByte();//读取远端校验和
                        if (l_crc == r_crc) {
                            //循环写入
                            for (i = 0; i < packlen; i++) {
                                if (addr + i >= 65536) {
                                    Terminal_putc_cb(CAN);
                                    Terminal_Printf("数据超界!\n");
                                    break;
                                }
                                // writeFlash(addr + i, buff[i]);
                            }
                            addr += packlen;
                            Terminal_putc_cb(ACK);
                        }
                        else {
                            Terminal_putc_cb(NAK);
                        }
                        break;
                }
            }
            Terminal_Printf("操作完毕!\n");
            break;
        default:
            break;
    }//End switch
    return -1;
}