#include "nvm.h"


static uint8_t NvM_Storage[NVM_BLOCK_COUNT][NVM_BLOCK_SIZE] = {
    {50U, 0U, 0U, 0U},
    {80U, 0U, 0U, 0U},
    {100U, 0U, 0U, 0U}
};


static uint8_t NvM_BlockChanged[NVM_BLOCK_COUNT] = {0U, 0U, 0U};

NvM_StatusType NvM_ReadBlock(uint8_t id, void *ramPtr)
{
    uint8_t *dst = (uint8_t *)ramPtr;


    if ((id < 1U) || (id > NVM_BLOCK_COUNT) || (ramPtr == 0))
    {
        return NVM_ERROR;
    }

    for (uint8_t i = 0U; i < NVM_BLOCK_SIZE; i++)
    {
        dst[i] = NvM_Storage[id - 1U][i];
    }
    return NVM_OK;
}

NvM_StatusType NvM_WriteBlock(uint8_t id, const void *ramPtr)
{
    const uint8_t *src = (const uint8_t *)ramPtr;

    if ((id < 1U) || (id > NVM_BLOCK_COUNT) || (ramPtr == 0))
    {
        return NVM_ERROR;
    }

    for (uint8_t i = 0U; i < NVM_BLOCK_SIZE; i++)
    {
        NvM_Storage[id - 1U][i] = src[i];
    }
    NvM_BlockChanged[id - 1U] = 0U;
    return NVM_OK;
}

NvM_StatusType NvM_SetRamBlockStatus(uint8_t id, uint8_t changed)
{
    if ((id < 1U) || (id > NVM_BLOCK_COUNT))
    {
        return NVM_ERROR;
    }
    NvM_BlockChanged[id - 1U] = (changed != 0U) ? 1U : 0U;
    return NVM_OK;
}
