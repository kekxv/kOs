/*C*****************************************************************************
* NAME:         AES_Lib.h
*-------------------------------------------------------------------------------
* Copyright (c) 杭州家和智能控制有限公司.
*-------------------------------------------------------------------------------
* RELEASE:      苏琦 suqiniat@163.com
* REVISION:     V1.0版
*-------------------------------------------------------------------------------
* PURPOSE:
* AES(Advanced Encryption Standard)加密算法
*
* NOTES:        仅支持128位加密标准
*******************************************************************************/

#ifndef AES_LIB_H
#define AES_LIB_H

/*_____ I N C L U D E S ______________________________________________________*/

#include <string.h>


/*_____ M A C R O S __________________________________________________________*/

//可移植的数据类型，方便程序的移植
typedef unsigned char AES_U8;
typedef unsigned int  AES_U16;

//存储在Flash中的存储类型定义(没有必要则用空格代替)
#define CODE_MEM_TYP

//仅支持128位加密标准
#define Nc (4)          // state数组和扩展密钥的列数

#define Nr (10)         // 加密轮数


/*_____ D E F I N I T I O N __________________________________________________*/

/*_____ D E C L A R A T I O N ________________________________________________*/

void AES_ExpandKey(AES_U8 *key, AES_U8 *expKey);  //扩展密钥加操作，相当于初始化AES算法
void AES_Encrypt(AES_U8 *in, AES_U8 *expKey, AES_U8 *out);  //128位加密函数
void AES_Decrypt(AES_U8 *in, AES_U8 *expKey, AES_U8 *out);	//128位解密函数


/******************************************************************************
                                使用范例
#include <stdio.h>
#include "AES_Lib.h"

AES_U8 in[16]  = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,  //输入16字节（128位）明文
                  0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};  

AES_U8 key[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,  //输入16字节（128位）密钥
                  0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};  
AES_U8 out[16];  //定义一个存入16个元素（128位）的数组，供密文输出

void main (void)  //程序从此处开始运行
{
AES_U8  expKey[4 * Nc * (Nr + 1)];  //定义一个放扩展密钥的数组，含有4*4*(10+1)个元素
AES_U16 idx;  //定义一个本地整型变量，作为数组索引

    AES_ExpandKey (key, expKey);  //密钥的扩展（算法的初始化）
    
    AES_Encrypt (in, expKey, out);  //对in明文数组进行加密，并通过out数组输出

    for (idx=0; idx<16; idx++)  //通过out数组输出密文
    {
        printf ("%.2x ", out[idx]);
    }
    printf ("\n");  //回车符

    AES_Decrypt (out, expKey, in);  //对out密文进行解密，并通过in数组输出
    
    for (idx=0; idx<16; idx++)  //通过in数组将解密后的明文输出
    {
        printf ("%.2x ", in[idx]);
    }
    printf ("\n");  //回车符
}
******************************************************************************/

#endif /*AES_LIB_H*/