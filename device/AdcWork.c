#include <rtthread.h>
#include <board.h>
#include <drv_common.h>
#include <rtdevice.h>
#include "adcwork.h"
#include "TdsWork.h"
#include "pin_config.h"
#include <stdio.h>

#define ADC_DEV_NAME        "adc1"      /* ADC 设备名称 */
#define ADC_DEV_CHANNEL     0           /* ADC 通道 */
#define REFER_VOLTAGE       330         /* 参考电压 3.3V,数据精度乘以100保留2位小数*/
#define CONVERT_BITS        (1 << 12)   /* 转换位数为12位 */
rt_adc_device_t adc_dev;

#define DBG_TAG "ADC"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void adc_init(void)
{
   /* 查找设备 */
   adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
   if (adc_dev == RT_NULL)
   {
       LOG_D("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
   }
   rt_adc_enable(adc_dev, 5);
   rt_adc_enable(adc_dev, 6);
   rt_adc_enable(adc_dev, 16);
}
MSH_CMD_EXPORT(adc_init,adc_init);
void ADC_Pin_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA0     ------> ADC1_IN5
    PA1     ------> ADC1_IN6
    PB1     ------> ADC1_IN16
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


}
void ADC_Pin_DeInit(void)
{
    __HAL_RCC_ADC_CLK_DISABLE();
    rt_pin_mode(ADC_BAT, PIN_MODE_OUTPUT);
    rt_pin_write(ADC_BAT,0);
    rt_pin_mode(ADC_DC, PIN_MODE_OUTPUT);
    rt_pin_write(ADC_DC,0);
    rt_pin_mode(ADC_TDS, PIN_MODE_OUTPUT);
    rt_pin_write(ADC_TDS,0);
    rt_pin_mode(ADC_MOTO, PIN_MODE_OUTPUT);
    rt_pin_write(ADC_MOTO,0);
}
uint32_t Get_Adc_Value(uint8_t channel)
{
    uint32_t value,sum=0;

    //rt_adc_enable(adc_dev, channel);
    /* 读取采样值 */
   for(uint8_t i=0;i<1;i++)
   {
       value = rt_adc_read(adc_dev, channel);
       //rt_kprintf("the value is :%d \r\n", value);
       sum+=value;
   }
   //sum = (int)(sum * REFER_VOLTAGE / CONVERT_BITS);
   //LOG_D("the voltage is :%d \r\n", sum);

   /* 关闭通道 */
   //rt_adc_disable(adc_dev, channel);
   return sum;
}
void testadc(void)
{
    Get_Adc_Value(5);
}
MSH_CMD_EXPORT(testadc,testadc);
uint32_t Get_Tds_Value(void)
{
//    uint32_t value,sum=0;
//    adc_get_offset();
//    for(uint8_t i=0;i<10;i++)
//    {
//            wm_adc_convert(adc_dc,2,&value);
//            sum+=value;
//    }
//    sum = sum*0.1;
//    sum = ((sum-8192.0)/8192*2.25/1.2 + 1.584)*1000;
//    printf("TDS Value is %d\r\n",sum);
//    return sum;
//    //5.4ms/cm -->1055(test1)

    return 0;
}
MSH_CMD_EXPORT(Get_Tds_Value,Get_Tds_Value);
uint8_t Get_DC_Level(void)
{
    uint32_t value;
	value  = Get_Adc_Value(5);
	LOG_D("DC Value is %ld\r\n",value);
    if(value>3000)return 1;
    else return 0;
}
MSH_CMD_EXPORT(Get_DC_Level,Get_DC_Level);
uint32_t Get_Bat_Value(void)
{
    uint32_t value;
    value  = Get_Adc_Value(6);
    LOG_D("BAT Value is %ld\r\n",value);
    return value;
}
uint8_t Get_Bat_Level(void)
{
    uint32_t value;
    value  = Get_Adc_Value(6);
    LOG_D("BAT Value is %ld\r\n",value);
    if(value>2000)return 1;
    else return 0;
}
MSH_CMD_EXPORT(Get_Bat_Level,Get_Bat_Level);
uint32_t Get_Moto_Value(void)
{
    uint32_t value;
    value  = Get_Adc_Value(16);
    LOG_D("Moto Value is %ld\r\n",value);
    return value;
//    if(value>20)return 1;
//    else return 0;
}
MSH_CMD_EXPORT(Get_Moto_Value,Get_Moto_Value);

