/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-06-01     Ernest       the first version
 */
#ifndef __PIN_CONFIG_H__
#define __PIN_CONFIG_H__


#define LED_R_PIN  				    21
#define LED_G_PIN  				    22
#define LED_B_PIN  				    23

//LCD
#define LCD_BL						42
#define LCD_CS						50
#define LCD_RST						19
#define LCD_SDA						43
#define LCD_CLK						44
#define LCD_EN                      15

//Key
#define K0							37
#define K1							36
#define K2							16

//MOTO.
#define MOTO_MODE					5
#define MOTO_VCC					4
#define MOTO_IN1					6
#define MOTO_IN2					7
#define MOTO_LEFT					24
#define MOTO_RIGHT				    25

#define Event_Moto_Free 		    (1 << 0)
#define Event_Moto_Forward 	        (1 << 1)
#define Event_Moto_Back 		    (1 << 2)
#define Event_Moto_Cycle		    (1 << 3)

//Flash
#define FLASH_MOSI                  31
#define FLASH_MISO                  30
#define FLASH_SCLK                  29
#define FLASH_CS                    28
#define FLASH_EN                    38

//ADC
#define ADC_BAT						1
#define ADC_DC						0
#define ADC_TDS						32
#define ADC_MOTO					17

//TDS
#define TDS_EN						35
#define TDS_CONTROL1			    34
#define TDS_CONTROL2			    33

//WIFI
#define WIFI_EN                     41
#define WIFI_RST                    8

//WATER FLOW
#define WATER_FLOW                  2

#endif /* __PIN_CONFIG_H__ */

