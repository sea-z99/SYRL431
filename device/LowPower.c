/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-08-27     Rick       the first version
 */
#include "rtthread.h"
#include "rtdevice.h"
#include "stm32l4xx.h"
#include "pin_config.h"
#include "motocontrol.h"
#include "DeviceInit.h"
#include "adcwork.h"
#include "flashwork.h"
#include "LowPower.h"
#include "TdsWork.h"
#include "RtcWork.h"

#define DBG_TAG "LowPower"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern void SystemClock_Config(void);
uint8_t Button_Wakeup_Flag=0;
void button_wakeup(void *parameter)
{
    Button_Wakeup_Flag=1;
}
void FlashInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_SPI2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    rt_pin_write(FLASH_EN,1);
}
void UsartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Alternate = 7;
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void FlashDeInit(void)
{
    rt_pin_write(FLASH_EN,0);
    __HAL_RCC_SPI2_CLK_DISABLE();
    rt_pin_mode(FLASH_MOSI, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_MOSI,0);
    rt_pin_mode(FLASH_MISO, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_MISO,0);
    rt_pin_mode(FLASH_SCLK, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_SCLK,0);
    rt_pin_mode(FLASH_CS, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_CS,0);
}
void WifiDeInit(void)
{
    rt_pin_mode(WIFI_EN, PIN_MODE_OUTPUT);
    rt_pin_write(WIFI_EN,0);
    rt_pin_mode(WIFI_RST, PIN_MODE_OUTPUT);
    rt_pin_write(WIFI_RST,0);
}
void get_water(void)
{
    LOG_D("WATER FLOW is %d",rt_pin_read(WATER_FLOW));
}
MSH_CMD_EXPORT(get_water,get_water);
void BeforSleep(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);

    rt_pin_attach_irq(K0, PIN_IRQ_MODE_FALLING, button_wakeup, RT_NULL);
    rt_pin_attach_irq(K1, PIN_IRQ_MODE_FALLING, button_wakeup, RT_NULL);
    rt_pin_attach_irq(K2, PIN_IRQ_MODE_FALLING, button_wakeup, RT_NULL);
    rt_pin_irq_enable(K0, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(K1, PIN_IRQ_ENABLE);
    rt_pin_irq_enable(K2, PIN_IRQ_ENABLE);

    //RGB
    Led_DeInit();
    //MOTO
    Moto_Pin_DeInit();
    //Flash
    FlashDeInit();
    //ADC
    ADC_Pin_DeInit();
    //WIFI
    WifiDeInit();
    //TDS
    Tds_DeInit();
}
void AfterWake(void)
{

    //RGB
    Led_Init();
    //MOTO
    Moto_Pin_Init();
    //Flash
    FlashInit();
    //ADC
    ADC_Pin_Init();
}
extern void RTC_Alarm_IRQHandler(void);
void LowPowerTimerStart(void);
void EnterLowPower(void)
{
//    BeforSleep();
//    Button_Wakeup_Flag=0;
//    LOG_D("Goto Stop Mode With RTC Now\r\n");
//    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
//    SystemClock_Config();
//    if(Button_Wakeup_Flag)
//    {
//        AfterWake();
//        LOG_D("Button Wake Up Now\r\n");
//    }
//    else
//    {
//        AfterWake();
//        LOG_D("RTC Wake Up Now\r\n");
//        //RTC_Timer_Entry();
//        //RTC_Check();
//        //EnterLowPower();
//        //LowPowerTimerStart();
//    }
}
MSH_CMD_EXPORT(EnterLowPower,EnterLowPower);
rt_timer_t LowPowerTimer=RT_NULL;
void LowPowerTimerCallback(void *parameter)
{
    EnterLowPower();
}
void LowPowerTimerStart(void)
{
    if(LowPowerTimer == RT_NULL)
    {
        LowPowerTimer = rt_timer_create("LowPowerTimer", LowPowerTimerCallback, RT_NULL, 1000, RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    }
    rt_timer_start(LowPowerTimer);
}
