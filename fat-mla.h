/******************************************************************************
 * FAT-MLA - FAT32 Medium Layer Adapter
 * Copyright (C) 2017-2018 Marco Giammarini
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

/******************************************************************************
 * @mainpage FAT-MLA FAT32 Medium Layer Adapter
 *
 * @section changelog ChangeLog
 *
 * @li v1.0.0 of 2018/01/04 - First release
 *
 * @section library External Library
 *
 * The library use the following external library
 * @li timer https://github.com/warcomeb/sdcard library for managing MMC/SD by
 * SPI protocol
 *
 ******************************************************************************/

#ifndef __WARCOMEB_FATMLA_H
#define __WARCOMEB_FATMLA_H

#define WARCOMEB_FATMLA_LIBRARY_VERSION     "1.0.0"
#define WARCOMEB_FATMLA_LIBRARY_VERSION_M   1
#define WARCOMEB_FATMLA_LIBRARY_VERSION_m   0
#define WARCOMEB_FATMLA_LIBRARY_VERSION_bug 0
#define WARCOMEB_FATMLA_LIBRARY_TIME        1470148734

#include "sdcard/sdcard.h"

/*
 * The user must define these label...
 */
#ifndef __NO_BOARD_H
#include "board.h"
#endif

#if !defined(FATMLA_DEVICE_SPI) && !defined(FATMLA_DEVICE_SDHC)
#error "Error: Communication type with SDCard is not defined!"
#endif

#if defined(FATMLA_DEVICE_SPI) && !defined(LIBOHIBOARD_SPI)
#error "Error: SPI must be enabled into libohiboard!"
#endif

#if defined(FATMLA_DEVICE_SPI) && defined(FATMLA_DEVICE_SDHC)
#error "Error: You must choose only one communication type!"
#endif

typedef struct _FATMLA_Config
{
#if defined(FATMLA_DEVICE_SPI)

    Spi_DeviceHandle   device;
    Gpio_Pins          csPin;
    Gpio_Pins          cpPin;                           /**< Card Present pin */
    int cpType;
    void (*delayTime)(uint32_t delay);       /**< Function for blocking delay */
    uint32_t (*currentTime)(void);             /**< Function for current time */

#elif defined(FATMLA_DEVICE_SDHC)

#endif

} FATMLA_Config;

void FATMLA_init (FATMLA_Config* config);

#endif /* __WARCOMEB_FATMLA_H */
