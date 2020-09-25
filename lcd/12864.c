/**
  * @file   UC1698Driver.c
  * @author  guoweilkd
  * @version V1.1.0
  * @date    2018/05/09
  * @brief  Lcd����
  * ���ļ�LcdΪUC1698
  */
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include "12864.h"
#include "pin_config.h"
#include "lkdgui.h"
#include "LowPower.h"

const uint8_t DataToBit2[] = {0x00,0x0f,0xf0,0xff};

uint8_t LCD_Flag=0;//0为亮屏可操作，1为亮屏不可操作，2为息屏有显示，3为彻底关断

extern char FirstFlag[30];
extern void windows_jump(void);
extern rt_thread_t lcd_task;
extern const unsigned char ASCII_5_7[95][5];

void delay(int i)
{
	 int j,k;
	 for(j=0;j<i;j++)
	 for(k=0;k<110;k++);
}

void delay_us(int i)
{
	 int j,k;
	 for(j=0;j<i;j++);
	 for(k=0;k<10;k++);

}
static void LcdGpioConfig(void)
{
    rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_CS, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RST, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_SDA, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_CLK, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_EN, PIN_MODE_OUTPUT);

    rt_pin_write(LCD_EN, 1);

    LCD_BL_HIGH();
}
MSH_CMD_EXPORT(LcdGpioConfig,LcdGpioConfig);
void testgpio(void)
{
    rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_CS, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RST, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_SDA, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_CLK, PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_EN, PIN_MODE_OUTPUT);

    rt_pin_write(LCD_BL, 0);
    rt_pin_write(LCD_CS, 1);
    rt_pin_write(LCD_RST, 1);
    rt_pin_write(LCD_SDA, 1);
    rt_pin_write(LCD_CLK, 1);
    rt_pin_write(LCD_EN, 1);
}
MSH_CMD_EXPORT(testgpio,testgpio);
/**
  *@brief д����
  *@param  cmd ����
  *@retval None
  */
void WriteLcdCommand(uint8_t cmd)
{
		LCD_CS_LOW();
		for(uint8_t i=0;i<8;i++)
		{
			LCD_CLK_LOW();
			if(cmd&0x80){LCD_SDA_HIGH();}
			else{LCD_SDA_LOW();}
			LCD_CLK_HIGH();
			delay_us(1);
			cmd<<=1;
		}
}

/**
  *@brief д����
  *@param  data ����
  *@retval None
  */
void WriteLcdData(uint8_t data)
{
		LCD_CS_HIGH();
		for(uint8_t i=0;i<8;i++)
		{
			LCD_CLK_LOW();
			if(data&0x80){LCD_SDA_HIGH();}
			else{LCD_SDA_LOW();}
			LCD_CLK_HIGH();
			delay_us(1);
			data<<=1;
		}
}

/**
  *@brief ������ʼ��
  *@param  row ��
  *@retval None
  */
void SetLcdRow(uint8_t row)
{
		WriteLcdCommand(0xb0+row);   				//����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ������ƽ����˵�ĵ�1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1*/
}

/**
  *@brief ������ʼ��
  *@param  col �е�ַ
  *@retval None
  */
void SetLcdCol(uint8_t col)
{ 							//����ƽ����˵�ĵ�1�У���LCD����IC���ǵ�0�С������������ȥ1.
		WriteLcdCommand(((col>>4)&0x0f)+0x10);	//�����е�ַ�ĸ�4λ
		WriteLcdCommand(col&0x0f);				//�����е�ַ�ĵ�4λ
}
void Write8Dots(uint8_t Data)
{
		WriteLcdData(Data);
}
void lcd_address(char page,char column)
{
	WriteLcdCommand(0xb0+page);   				//����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ������ƽ����˵�ĵ�1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1*/
	WriteLcdCommand(((column>>4)&0x0f)+0x10);	//�����е�ַ�ĸ�4λ
	WriteLcdCommand(column&0x0f);				//�����е�ַ�ĵ�4λ
}
void lcd_x_address(char column)
{
	WriteLcdCommand(((column>>4)&0x0f)+0x10);	//�����е�ַ�ĸ�4λ
	WriteLcdCommand(column&0x0f);				//�����е�ַ�ĵ�4λ
}
void lcd_y_address(char page)
{
	WriteLcdCommand(0xb0+page);   				//����ҳ��ַ��ÿҳ��8�С�һ�������64�б��ֳ�8��ҳ������ƽ����˵�ĵ�1ҳ����LCD����IC���ǵ�0ҳ�������������ȥ1*/
}
void clear_screen(void)
{
    uint8_t i,j;
    for(i=0;i<8;i++)
    {
        //lcd_y_address(i);
        lcd_address(i,0);
        for(j=0;j<128;j++)
        {
            //lcd_x_address(j);
            //lcd_address(i,j);
            WriteLcdData(0);
        }
    }
}
MSH_CMD_EXPORT(clear_screen,clear_screen);

void full_screen(void)
{
    uint8_t i,j;
    for(i=0;i<8;i++)
    {
        lcd_address(i,0);
        for(j=0;j<128;j++)
        {
            WriteLcdData(0xff);
        }
    }
}
MSH_CMD_EXPORT(full_screen,full_screen);

void display_string_5x7(uint8_t page,uint8_t column,char *text)
{
	uint8_t i=0,j,k;
	while(text[i]>0x00)
	{
		if((text[i]>=0x20)&&(text[i]<0x7e))
		{
			j=text[i]-0x20;
			SetLcdRow(page);
			SetLcdCol(column);
			for(k=0;k<5;k++)
			{
				WriteLcdData(ASCII_5_7[j][k]);/*��ʾ5x7��ASCII�ֵ�LCD�ϣ�yΪҳ��ַ��xΪ�е�ַ�����Ϊ����*/
			}
			i++;
			column+=6;
		}
		else
		i++;
	}
}
void display_test(void)
{
		LcdInit();
		full_screen();

}
MSH_CMD_EXPORT(display_test,display_test);
/**
  *@brief ����ʾ
  *@param  None
  *@retval None
  */

void OpenLcdDisplay(void)
{
    switch(LCD_Flag)
    {
        case 1:
            LCD_Flag=0;
            break;
        case 2:
            LCD_Flag=1;
            LCD_BL_HIGH();
            break;
        case 3:
            LCD_Flag=1;
            LcdInit();
            break;
        default:break;
    }
}

void LcdDeinit(void)
{
    rt_pin_mode(LCD_EN, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_EN,0);
}
void CloseLcdVcc(void)
{
    LCD_Flag = 3;
    rt_pin_write(LCD_CS,0);
    rt_pin_write(LCD_RST,0);
    rt_pin_mode(LCD_SDA, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_SDA,0);
    rt_pin_write(LCD_CLK,0);
    LcdDeinit();
    EnterLowPower();
}
void CloseLcdBacklight(void)
{
    LCD_Flag = 2;
    LCD_BL_LOW();
}
void BlinkBL(void)
{
    rt_thread_mdelay(500);
    LCD_BL_HIGH();
    rt_thread_mdelay(1000);
    LCD_BL_LOW();
    rt_thread_mdelay(500);
    LCD_BL_HIGH();
    rt_thread_mdelay(1000);
    LCD_BL_LOW();
}
/**
  *@brief LCD��ʼ��
  *@param  None
  *@retval None
  */
void LcdInit(void)
{
    LCD_EN_HIGH();
	LcdGpioConfig();
	LCD_RST_LOW();
	delay_us(3);
	LCD_RST_HIGH();
	delay(20);
	WriteLcdCommand(0xe2);
	delay(5);
	WriteLcdCommand(0x2c);
	delay(5);
	WriteLcdCommand(0x2e);
	delay(5);
	WriteLcdCommand(0x2f);
	delay(5);
	WriteLcdCommand(0x24);
	WriteLcdCommand(0x81);
	WriteLcdCommand(0x15);
	WriteLcdCommand(0xa2);
	WriteLcdCommand(0xc8);
	WriteLcdCommand(0xa0);
	WriteLcdCommand(0x40);
	WriteLcdCommand(0xaf);
	clear_screen();
    LCD_BL_HIGH();
}
