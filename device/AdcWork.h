#ifndef __ADCWORK_H__
#define __ADCWORK_H__
#endif
#include <stdint.h>
void adc_init(void);
void ADC_Pin_Init(void);
void ADC_Pin_DeInit(void);
void adc_work(void);
uint8_t Get_Bat_Level(void);
uint8_t Get_DC_Level(void);
uint32_t Get_Tds_Value(void);
uint32_t Get_Moto_Value(void);

