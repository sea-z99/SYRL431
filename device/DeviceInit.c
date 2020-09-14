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

rt_sem_t K0_Sem = RT_NULL;
rt_sem_t K1_Sem = RT_NULL;
rt_sem_t K2_Sem = RT_NULL;
rt_sem_t K2_Long_Sem = RT_NULL;

rt_thread_t button_task=RT_NULL;
rt_timer_t Screen_Backlight_Timer=RT_NULL;
rt_timer_t Screen_Vcc_Timer=RT_NULL;

uint8_t K2_Long_Counter=0;
uint8_t K2_Long_Flag=0;
uint8_t Delta_press_work_Flag=0;

extern struct rt_event Moto_Event;
extern uint8_t PowerScreen_Flag;
extern uint8_t LCD_Flag;
extern uint8_t Setting_Deltapress; //0:Report only  1:Automatic
extern uint8_t Deltapress_Enable;//0:close   1:open
extern uint16_t Counter_Deltapress;

#define DBG_TAG "Button"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

void Light_Red(void)
{
	rt_pin_mode(LED_G_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(LED_G_PIN,1);
	rt_pin_mode(LED_R_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(LED_R_PIN,0);
}
MSH_CMD_EXPORT(Light_Red,Light_Red);
void Light_BackLight(void)
{
	rt_pin_mode(LCD_BL, PIN_MODE_OUTPUT);
	rt_pin_write(LCD_BL,1);
}
MSH_CMD_EXPORT(Light_BackLight,Light_BackLight);
void Key_Sem_init(void)
{
	K0_Sem=rt_sem_create("K0", 0, RT_IPC_FLAG_FIFO);
	K1_Sem=rt_sem_create("K1", 0, RT_IPC_FLAG_FIFO);
	K2_Sem=rt_sem_create("K2", 0, RT_IPC_FLAG_FIFO);
	K2_Long_Sem = rt_sem_create("K2_Long", 0, RT_IPC_FLAG_FIFO);
}
void K0_Sem_Release(void *parameter)
{
	OpenLcdDisplay();
	ScreenTimerRefresh();
	LOG_D("Back Key is Press\r\n");
	if(LCD_Flag==0)
	{
		rt_sem_release(K0_Sem);
	}
}
void K1_Sem_Release(void *parameter)
{
	OpenLcdDisplay();
    ScreenTimerRefresh();
	LOG_D("Down Key is Press\r\n");
	if(LCD_Flag==0)
	{
		rt_sem_release(K1_Sem);
	}
}
void K2_Sem_Release(void *parameter)
{
	OpenLcdDisplay();
    ScreenTimerRefresh();
	LOG_D("Enter Key is Press\r\n");
	if(LCD_Flag==0)
	{
		rt_sem_release(K2_Sem);
	}
}

void K2_Long_Sem_Release(void *parameter)
{
	OpenLcdDisplay();
    ScreenTimerRefresh();
	if(K2_Long_Flag == 1)
	{
			return;
	}
	LOG_D("The Long Counter is %d\r\n",K2_Long_Counter);
	K2_Long_Counter++;
	if(K2_Long_Counter>10)
	{
		K2_Long_Flag=1;
		K2_Long_Counter=0;
		rt_sem_release(K2_Long_Sem);
	}
}
void K2_Long_Free_Release(void *parameter)
{
	OpenLcdDisplay();
    ScreenTimerRefresh();
	K2_Long_Flag=0;
	LOG_D("Enter Key is Long Free\r\n");
	K2_Long_Counter=0;
}
uint8_t Read_K0_Level(void)
{
  return rt_pin_read(K0);
}
uint8_t Read_K1_Level(void)
{
  return rt_pin_read(K1);
}
uint8_t Read_K2_Level(void)
{
  return rt_pin_read(K2);
}
void LcdVccTimerCallback(void *parameter)
{
    LOG_D("Lcd Vcc Timer is come\r\n");
    LCD_Flag = 3;
    CloseLcdVcc();
}
void LcdBacklightTimerCallback(void *parameter)
{
    LOG_D("Lcd Backlight Timer is come\r\n");
    LCD_Flag = 2;
    CloseLcdBacklight();
}
void SreenTimerInit(void)
{
    Screen_Backlight_Timer=rt_timer_create("Lcd_Backlight_Timer",LcdBacklightTimerCallback,RT_NULL,50*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    if(Screen_Backlight_Timer!=RT_NULL)
    {
        rt_timer_start(Screen_Backlight_Timer);
    }
    Screen_Vcc_Timer=rt_timer_create("Lcd_Vcc_Timer",LcdVccTimerCallback,RT_NULL,100*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    if(Screen_Vcc_Timer!=RT_NULL)
    {
        rt_timer_start(Screen_Vcc_Timer);
    }
}
void ScreenTimerStop(void)
{
    rt_timer_stop(Screen_Backlight_Timer);
    rt_timer_stop(Screen_Vcc_Timer);
}
void ScreenTimerRefresh(void)
{
    rt_timer_start(Screen_Backlight_Timer);
    rt_timer_start(Screen_Vcc_Timer);
}
void Led_Init(void)
{
    rt_pin_mode(LED_G_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_G_PIN,1);
    rt_pin_mode(LED_R_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_R_PIN,1);
    rt_pin_mode(LED_B_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_B_PIN,1);
}
void Led_DeInit(void)
{
    rt_pin_mode(LED_G_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_G_PIN,1);
    rt_pin_mode(LED_R_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_R_PIN,1);
    rt_pin_mode(LED_B_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_B_PIN,1);
}
void Green_Light(void)
{
    rt_pin_write(LED_G_PIN,0);
}
void Green_Off(void)
{
    rt_pin_write(LED_G_PIN,1);
}
void Red_Light(void)
{
    rt_pin_write(LED_R_PIN,0);
}
void Red_Off(void)
{
    rt_pin_write(LED_R_PIN,1);
}
void Delta_press(void *parameter)
{
    Delta_press_work_Flag = 1;
}
void Key_Init(void)
{
    Key_Sem_init();
    SreenTimerInit();
    rt_pin_mode(K0, PIN_MODE_INPUT);
    rt_pin_mode(K1, PIN_MODE_INPUT);
    rt_pin_mode(K2, PIN_MODE_INPUT);
    rt_pin_mode(MOTO_LEFT, PIN_MODE_INPUT);
    rt_pin_mode(MOTO_RIGHT, PIN_MODE_INPUT);
    rt_pin_mode(WATER_FLOW, PIN_MODE_INPUT);
    rt_pin_attach_irq(WATER_FLOW, PIN_IRQ_MODE_FALLING, Delta_press, RT_NULL);
    rt_pin_irq_enable(WATER_FLOW, PIN_IRQ_ENABLE);
}
void button_task_entry(void *parameter)
{
    Key_Init();
    Button_t Key0;
    Button_t Key1;
    Button_t Key2;
    Button_Create("Key0",&Key0,Read_K0_Level,0);
    Button_Create("Key1",&Key1,Read_K1_Level,0);
    Button_Create("Key2",&Key2,Read_K2_Level,0);
    Button_Attach(&Key0,BUTTON_DOWM,K0_Sem_Release);
    Button_Attach(&Key1,BUTTON_DOWM,K1_Sem_Release);
    Button_Attach(&Key1,BUTTON_LONG,K1_Sem_Release);
    Button_Attach(&Key2,BUTTON_DOWM,K2_Sem_Release);
    Button_Attach(&Key2,BUTTON_LONG,K2_Long_Sem_Release);
    Button_Attach(&Key2,BUTTON_LONG_FREE,K2_Long_Free_Release);
    while(1)
    {
        Button_Process();
        rt_thread_mdelay(10);
    }
}
void button(void)
{
    button_task=rt_thread_create("button_task",button_task_entry,RT_NULL,1024,5,20);
    if(button_task!=RT_NULL)rt_thread_startup(button_task);
}

void Delta_press_work(void)
{
    if(Delta_press_work_Flag)
    {
        Delta_press_work_Flag=0;
        if(Deltapress_Enable)
        {
            LOG_D("Detected Water Flow\r\n");
            Counter_Deltapress++;
            Flash_Set(10,Counter_Deltapress);
            OpenLcdDisplay();
            if(Setting_Deltapress)
            {
                JumptoAutomatic();
            }
            else
            {
                JumptoReminder();
            }
        }
    }
}
void delta_test(void)
{
    rt_kprintf("Delta is %d\r\n",rt_pin_read(65));
}
MSH_CMD_EXPORT(delta_test,delta_test)
