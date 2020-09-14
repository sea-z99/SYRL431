#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "TdsWork.h"
#include "adcwork.h"


void Tds_Init(void)
{
		rt_pin_mode(TDS_EN, PIN_MODE_OUTPUT);
		rt_pin_mode(TDS_CONTROL1, PIN_MODE_OUTPUT);
		rt_pin_mode(TDS_CONTROL2, PIN_MODE_OUTPUT);
		rt_pin_write(TDS_EN,0);
		rt_pin_write(TDS_CONTROL1,0);
		rt_pin_write(TDS_CONTROL2,0);
}
void Tds_DeInit(void)
{
        rt_pin_mode(TDS_EN, PIN_MODE_OUTPUT);
        rt_pin_write(TDS_EN,0);
        rt_pin_mode(TDS_CONTROL1, PIN_MODE_OUTPUT);
        rt_pin_write(TDS_CONTROL1,0);
        rt_pin_mode(TDS_CONTROL2, PIN_MODE_OUTPUT);
        rt_pin_write(TDS_CONTROL2,0);
}
uint32_t Tds_Work(void)
{
		uint32_t result,sum=0;
		Tds_Init();
		for(uint8_t i=0;i<10;i++)
		{
			if(i%2==0)
			{
				rt_pin_write(TDS_EN,1);
				rt_pin_write(TDS_CONTROL1,0);
				rt_pin_write(TDS_CONTROL2,1);
				rt_thread_mdelay(100);
				result = Get_Tds_Value();
				sum += result;
			}
			else
			{
				rt_pin_write(TDS_CONTROL1,1);
				rt_pin_write(TDS_CONTROL2,0);
				rt_pin_write(TDS_EN,0);
				rt_thread_mdelay(100);
			}
		}
		sum = (uint32_t)(sum*0.2);
		rt_kprintf("TDS Work Value is %d\r\n",sum);
		return sum;
}
MSH_CMD_EXPORT(Tds_Work,Tds_Work);
