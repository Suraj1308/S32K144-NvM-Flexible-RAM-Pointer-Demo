#ifndef NVM_H
#define NVM_H

#include <stdint.h>


#define RAM_BLOCK        0

#define NVM_BLOCK_COUNT  3U
#define NVM_BLOCK_SIZE   4U

typedef enum
{
    NVM_OK = 0,
    NVM_ERROR = 1
} NvM_StatusType;

NvM_StatusType NvM_ReadBlock(uint8_t id, void *ramPtr);
NvM_StatusType NvM_WriteBlock(uint8_t id, const void *ramPtr);
NvM_StatusType NvM_SetRamBlockStatus(uint8_t id, uint8_t changed);

#endif
