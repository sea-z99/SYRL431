/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-21     RT-Thread    first version
 */

#include <rtthread.h>
#include "lcd_display.h"
#include <easyflash.h>
#include <fal.h>
#include "DeviceInit.h"
#include "lcd_display.h"
#include "MotoControl.h"
#include "adcwork.h"
#include "rtcwork.h"
#include "LowPower.h"
#include "FlashWork.h"
#include "Battery.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    flash_Init();
    fal_init();
    easyflash_init();
    boot_time();
    RTC_Init();
    Led_Init();
    button();
    adc_init();
    lcd();
    Moto_TaskUp();
    BatterWatcher();
    while (1)
    {
        //EnterLowPower();
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
