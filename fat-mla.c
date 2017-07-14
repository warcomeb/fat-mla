/******************************************************************************
 * FAT-MLA - FAT32 Medium Layer Adapter
 * Copyright (C) 2017 Marco Giammarini
 *
 * Authors:
 *  Marco Giammarini <m.giammarini@warcomeb.it>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#include "fat-mla.h"

#include "diskio.h"     /* FatFs lower layer API */

#ifdef WARCOMEB_SDCARD_DEBUG
#include "../cli/cli.h"
#endif

#define FATMLA_FAT_BLOCK_SIZE   512

#ifdef FATMLA_DEVICE_SPI
static SDCard_Device FATMLA_card;
#else

#endif

/**
 * This variable is used to save the status of init function.
 * TRUE if user call init function before use file system, FALSE
 * otherwise.
 */
static bool FATMLA_isConfigured = FALSE;

void FATMLA_init (FATMLA_Config* config)
{
#ifdef FATMLA_DEVICE_SPI
    FATMLA_card.device      = config->device;
    FATMLA_card.csPin       = config->csPin;
    FATMLA_card.cpPin       = config->cpPin;
    FATMLA_card.cpType      = config->cpType;
    FATMLA_card.delayTime   = config->delayTime;
    FATMLA_card.currentTime = config->currentTime;
#else
#endif

    FATMLA_isConfigured = TRUE;
}

/******************************************************************************
 * Media access interface for FatFs Module defined in diskio.h
 ******************************************************************************/

DSTATUS disk_initialize (BYTE pdrv)
{
#ifdef FATMLA_DEVICE_SPI
    SDCard_Errors error;
#else

#endif

    // Supports only drive 0 (SD Card)
    if (pdrv) return STA_NOINIT;

    // Return error when init fuction didn't call
    if (!FATMLA_isConfigured) return STA_NOINIT;

#ifdef FATMLA_DEVICE_SPI

    error = SDCard_init(&FATMLA_card);

    if (error != SDCARD_ERRORS_OK)
        return STA_NOINIT;
    else
        return 0;
#else

#endif
}

DSTATUS disk_status (BYTE pdrv)
{
    // Supports only drive 0 (SD Card)
    if (pdrv) return STA_NOINIT;

    if (FATMLA_card.isInit != TRUE) return STA_NOINIT;

    return 0;
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	SDCard_Errors error;
	uint8_t retry = 0;
    if ((pdrv != 0) || !count) return RES_PARERR;
    if (FATMLA_card.isInit != TRUE) return RES_NOTRDY;

    // Single block read
    if (count == 1)
    {
//        Cli_sendString("R1");
        do
        {
//            Cli_sendString("R11");
            error = SDCard_readBlock(&FATMLA_card,sector,(uint8_t*)buff);
            retry++;
            if (retry > 10)
            {
                return RES_ERROR;
            }
            FATMLA_card.delayTime(100); // 100ms
        } while (error != SDCARD_ERRORS_OK);
//        if (SDCard_readBlock(&FATMLA_card,sector,(uint8_t*)buff) != SDCARD_ERRORS_OK)
//        {
//            return RES_ERROR;
//        }
        count = 0;
    }
    // Read multiple blocks
    else
    {
//        Cli_sendString("RN");
        do
        {
            error = SDCard_readBlocks(&FATMLA_card,sector,(uint8_t*)buff,count);
            retry++;
            if (retry > 10)
            {
                return RES_ERROR;
            }
            FATMLA_card.delayTime(100); // 100ms
        } while (error != SDCARD_ERRORS_OK);
//        if (SDCard_readBlocks(&FATMLA_card,sector,(uint8_t*)buff,count) != SDCARD_ERRORS_OK)
//        {
//            return RES_ERROR;
//        }
        count = 0;
    }
    return count ? RES_ERROR : RES_OK;
}

DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	SDCard_Errors error;
	uint8_t retry = 0;
    if ((pdrv != 0) || !count) return RES_PARERR;
    if (FATMLA_card.isInit != TRUE) return RES_NOTRDY;

    const BYTE* tempBuff = buff;
    // Single block write
    if (count == 1)
    {
//    	Cli_sendString("W1");
        do
        {
            error = SDCard_writeBlock(&FATMLA_card,sector,(uint8_t*)buff);
            retry++;
//            Cli_sendString("W11");
            if (retry > 10)
            {
                return RES_ERROR;
            }
            FATMLA_card.delayTime(100); // 100ms
        } while (error != SDCARD_ERRORS_OK);
//        if (SDCard_writeBlock(&FATMLA_card,sector,(uint8_t*)buff) != SDCARD_ERRORS_OK)
//        {
//            return RES_ERROR;
//        }
        count = 0;
	}
    // Write multiple blocks
    else
    {
//    	Cli_sendString("WN");
        do
        {
            error = SDCard_writeBlocks(&FATMLA_card,sector,(uint8_t*)buff,count);
            retry++;
            if (retry > 10)
            {
                return RES_ERROR;
            }
            FATMLA_card.delayTime(100); // 100ms
        } while (error != SDCARD_ERRORS_OK);
//        if (SDCard_writeBlocks(&FATMLA_card,sector,(uint8_t*)buff,count) != SDCARD_ERRORS_OK)
//        {
//            return RES_ERROR;
//        }
        count = 0;
    }
    return count ? RES_ERROR : RES_OK;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
    DRESULT res;
    uint32_t size = 0;

    if (pdrv != 0) return RES_PARERR;
    if (FATMLA_card.isInit != TRUE) return RES_NOTRDY;

    res = RES_ERROR;

    switch (cmd)
    {
    case CTRL_SYNC:
        if (SDCard_isBusy(&FATMLA_card) == FALSE) res = RES_OK;
        else res = RES_ERROR;
        break;

    // Returns number of available sectors on the drive
    case GET_SECTOR_COUNT:
        if (SDCard_getSectorCount(&FATMLA_card,&size) != SDCARD_ERRORS_OK)
        {
            res = RES_PARERR;
            break;
        }
        else
        {
            *(DWORD*)buff = size;
        }
        break;

    // Returns sector size of the device
    case GET_SECTOR_SIZE:
        *(WORD*)buff = FATMLA_FAT_BLOCK_SIZE;
        break;

    // Returns erase block size of the flash memory media in unit of sector
    case GET_BLOCK_SIZE:
        // NYI
        *(DWORD*)buff = 0;
        res = RES_PARERR;
        break;

    case CTRL_TRIM:
        // NYI
        *(DWORD*)buff = 0;
        res = RES_PARERR;
        break;

    default:
        res = RES_PARERR;
    }

    return res;
}

DWORD get_fattime (void)
{
    return 0;
}
