/*********************************
* Date: 2010-08-28
* Author: stanbaek
*********************************/

#ifndef __FLASHMEM_H
#define __FLASHMEM_H


// Page 0 will be used to store the index of 
//
#define FLASH_IMU_PAGE_INDEX_LOC     0x00
#define FLASH_IMU_PAGE_INDEX_SIZE    4
#define FLASH_IMU_BYTE_INDEX_LOC     0x00
#define FLASH_IMU_BYTE_INDEX_SIZE    4

// IMU data will be saved from Page 1 to the end.
//
#define FLASH_IMU_PAGE_START_LOC     0x0100
#define FLASH_IMU_BYTE_START_LOC     0


#endif  // __FLASHMEM_H


