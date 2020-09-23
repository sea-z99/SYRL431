/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-09-23     Rick       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "DeviceInit.h"
#include "pin_config.h"
#include "button.h"
#include "Flashwork.h"
#include "lcd_display.h"
#include "12864.h"
#include "LowPower.h"
#include "motocontrol.h"
#include "Battery.h"
#include "adcwork.h"

#define DBG_TAG "Battery"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint32_t NowDcVol=0;
uint32_t NowBatVol=0;
uint32_t PastBatVol=0;
uint8_t LowVoltageFlag=0;
rt_thread_t BatterWatcher_Thread=RT_NULL;
void BatteryWatcherCallback(void *parameter)
{
    LOG_D("Battery Watcher Init OK\r\n");
    while(1)
    {
        PastBatVol = NowBatVol;
        NowBatVol = Get_Bat_Value();
        //NowDcVol = Get_DC_Level();

        //if(NowDcVol == 0)
        //{
            if(LowVoltageFlag)
            {
                if(NowBatVol>PastBatVol&&NowBatVol-PastBatVol>200)
                {
                    if(NowBatVol>3429)
                    {
                        LowVoltageFlag = 0;
                        LOG_D("New Battery is OK\r\n");
                    }
                    else
                    {
                        LowVoltageFlag = 0;
                        LOG_D("New Battery is BAD\r\n");
                        JumpToBatteryNew();
                    }
                }
            }
            else
            {
                if(NowBatVol<2345)
                {
                    LOG_D("Battery is Empty\r\n");
                    LowVoltageFlag=1;
                    JumpToBatteryEmpty();
                }
            }
        //}
        rt_thread_mdelay(5000);
    }
}
void BatterWatcher(void)
{
    BatterWatcher_Thread = rt_thread_create("BatterWatcher", BatteryWatcherCallback, RT_NULL, 1024, 30, 10);
    if(BatterWatcher_Thread!=RT_NULL)rt_thread_startup(BatterWatcher_Thread);
}
MSH_CMD_EXPORT(BatterWatcher,BatterWatcher);
