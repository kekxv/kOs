//
// Created by caesar kekxv on 2020/9/23.
//
#include "flash_rw.h"
#include "main.h"
#include <stdio.h>
#include <string.h>


HAL_StatusTypeDef FLASH_RW_erase(int pageIndex) {
    int page_size = FLASH_RW_PAGESIZE;
    uint32_t addr = FLASH_RW_GET_PAGE_INDEX_ADDR(pageIndex, page_size);
    //1、解锁FLASH
    HAL_StatusTypeDef ret = HAL_FLASH_Unlock();
    if (HAL_OK != ret) {
        return ret;
    }
    //2、擦除FLASH
    //初始化FLASH_EraseInitTypeDef
    FLASH_EraseInitTypeDef f;
    f.TypeErase = FLASH_TYPEERASE_PAGES;
    f.PageAddress = addr;
    f.NbPages = 1;
    //设置PageError
    uint32_t PageError = 0;
    //调用擦除函数
    ret = HAL_FLASHEx_Erase(&f, &PageError);
    if (HAL_OK != ret) {
        HAL_FLASH_Lock();
        return ret;
    }
    //4、锁住FLASH
    return HAL_FLASH_Lock();
}
HAL_StatusTypeDef FLASH_RW_Write(int pageIndex, const uint8_t *data, uint32_t data_size, uint32_t offset) {
    if (offset % 2 != 0 || data_size % 2 != 0) {
        return HAL_ERROR;
    }
    int page_size = FLASH_RW_PAGESIZE;
    uint32_t addr = FLASH_RW_GET_PAGE_INDEX_ADDR(pageIndex, page_size) + offset;
    //1、解锁FLASH
    HAL_StatusTypeDef ret = HAL_FLASH_Unlock();
    if (HAL_OK != ret) {
        return ret;
    }
    // TODO
    //3、对FLASH烧写
    for (uint32_t i = 0; i < data_size; i += 2) {
        ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, *(uint16_t *) &data[i]);
        if (HAL_OK != ret) {
            break;
        }
    }

    //4、锁住FLASH
    HAL_StatusTypeDef ret1 = HAL_FLASH_Lock();
    if (HAL_OK != ret) return ret;
    return ret1;
}

HAL_StatusTypeDef FLASH_RW_Read(int pageIndex, uint8_t *data, uint32_t data_size, uint32_t offset) {
    int page_size = FLASH_RW_PAGESIZE;
    uint8_t *addr = (uint8_t *) FLASH_RW_GET_PAGE_INDEX_ADDR(pageIndex, page_size);
    memcpy(data, addr + offset, data_size);
    return HAL_OK;
}

