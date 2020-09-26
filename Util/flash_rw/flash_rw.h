//
// Created by caesar kekxv on 2020/9/23.
//

#ifndef KOS_FLASH_RW_H
#define KOS_FLASH_RW_H
#include "main.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_flash.h"

#define FLASH_RW_PAGESIZE FLASH_PAGE_SIZE

#define FLASH_RW_GET_PAGE_INDEX_ADDR(index, page_size) (index * page_size + FLASH_BASE)

/**
 * 擦除数据
 * @param pageIndex
 * @return
 */
HAL_StatusTypeDef FLASH_RW_erase(int pageIndex);

/**
 * 写入数据
 * @param pageIndex
 * @param data
 * @param data_size 必须为2的倍数
 * @param offset 必须为2的倍数
 * @return
 */
HAL_StatusTypeDef FLASH_RW_Write(int pageIndex, const uint8_t *data, uint32_t data_size, uint32_t offset);
/**
 * 读取数据
 * @param pageIndex
 * @param data
 * @param data_size
 * @param offset
 * @return
 */
HAL_StatusTypeDef FLASH_RW_Read(int pageIndex, uint8_t *data, uint32_t data_size, uint32_t offset);

#endif //KOS_FLASH_RW_H
