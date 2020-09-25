#include "rtthread.h"
#include "rtdevice.h"
#include "stm32l4xx.h"
#include "MotoControl.h"
#include "pin_config.h"
#include "lcd_display.h"
#include "adcwork.h"
#include "deviceinit.h"
#include "12864.h"
static rt_thread_t Moto_Thread=RT_NULL;
struct rt_event Moto_Event;
rt_timer_t Moto_Cycle_Timer=RT_NULL;

uint8_t MotoWorkFlag=0;

#define DBG_TAG "MOTO"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

extern uint16_t Setting_Backwashtime;
extern uint8_t LowVoltageFlag;
extern uint32_t RTC_Reminder_Time ;
extern uint32_t RTC_Automatic_Time;

void Moto_Cycle_Timer_Callback(void *parameter)
{
    if(rt_pin_read(MOTO_RIGHT)==0)
    {
        LOG_D("Moto Start Back\r\n");
        rt_event_send(&Moto_Event, Event_Moto_Back);
    }
    else
    {
        LOG_D("No Moto,Start to Free\r\n");
        rt_event_send(&Moto_Event, Event_Moto_Free);
        ScreenTimerRefresh();
        jumpd();
    }
}
void Moto_Pin_Init(void)
{
    rt_pin_mode(MOTO_MODE, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_MODE,0);
    rt_pin_mode(MOTO_IN1, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN1,0);
    rt_pin_mode(MOTO_IN2, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN2,0);
    rt_pin_mode(MOTO_VCC, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_VCC,1);
    rt_pin_mode(MOTO_LEFT, PIN_MODE_INPUT);
    rt_pin_mode(MOTO_RIGHT, PIN_MODE_INPUT);

}
void Moto_Pin_DeInit(void)
{
    rt_pin_mode(MOTO_MODE, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_MODE,0);
    rt_pin_mode(MOTO_IN1, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN1,0);
    rt_pin_mode(MOTO_IN2, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_IN2,0);
    rt_pin_mode(MOTO_VCC, PIN_MODE_OUTPUT);
    rt_pin_write(MOTO_VCC,0);
    rt_pin_mode(MOTO_LEFT, PIN_MODE_INPUT);
    rt_pin_mode(MOTO_RIGHT, PIN_MODE_INPUT);
}
void Moto_Init(void)
{
    rt_event_init(&Moto_Event, "Moto_Event", RT_IPC_FLAG_FIFO);
    Moto_Cycle_Timer= rt_timer_create("Moto_Cycle_Timer",Moto_Cycle_Timer_Callback,RT_NULL,15*1000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
    Moto_Pin_Init();
    LOG_D("Moto Init Success\r\n");
}
void moto_forward(void)
{
		rt_event_send(&Moto_Event, Event_Moto_Forward);
}
MSH_CMD_EXPORT(moto_forward,moto_forward);
void moto_back(void)
{
		rt_event_send(&Moto_Event, Event_Moto_Back);
}
MSH_CMD_EXPORT(moto_back,moto_back);
void moto_stop(void)
{
		rt_event_send(&Moto_Event, Event_Moto_Free);
}
MSH_CMD_EXPORT(moto_stop,moto_stop);

void Moto_Cycle(void)
{
    RTC_Reminder_Time = 0;
    RTC_Automatic_Time = 0;
    if(LowVoltageFlag==0)
    {
        if(MotoWorkFlag == 0)
        {
            ScreenTimerStop();
            uint32_t Setting_Backwashtime_MileSecond=0;
            rt_event_send(&Moto_Event, Event_Moto_Forward);
            Setting_Backwashtime_MileSecond = Setting_Backwashtime*1000;
            rt_timer_control(Moto_Cycle_Timer,RT_TIMER_CTRL_SET_TIME,&Setting_Backwashtime_MileSecond);
            LOG_D("Start Backwash with Timer %d\r\n",Setting_Backwashtime);
            rt_timer_start(Moto_Cycle_Timer);
            Green_Light();
        }
        else
        {
            LOG_D("Moto is Working Now");
        }
    }
    else
    {
        LOG_D("Moto Not Work(Low Voltage)");
        jumpe();
        JumpToBatteryEmpty();
    }
}
MSH_CMD_EXPORT(Moto_Cycle,Moto_Cycle);
uint8_t Left_Status,Right_Status=0;;
void Moto_Free_Event_Release(void)
{
    Left_Status = rt_pin_read(MOTO_LEFT);
    Right_Status = rt_pin_read(MOTO_RIGHT);
    if(Right_Status==0&&MotoWorkFlag==1)
    {
        rt_event_send(&Moto_Event, Event_Moto_Free);
        rt_pin_write(MOTO_IN1,0);
        rt_pin_write(MOTO_IN2,0);
        MotoWorkFlag=0;
        LOG_D("Right is Done\r\n");
    }
    if(Left_Status==0&&MotoWorkFlag==2)
    {
        rt_pin_write(MOTO_IN1,0);
        rt_pin_write(MOTO_IN2,0);
        MotoWorkFlag=0;
        ScreenTimerRefresh();
        jumpc();
        LOG_D("Left is Done\r\n");
    }
}
void SleepAfterCycle(void)
{
        Moto_Cycle();
}
void Moto_Entry(void *parameter)
{
    rt_uint32_t e;
    rt_uint32_t Moto_Voltage=0;
    Moto_Init();
    while(1)
    {
        if(MotoWorkFlag>0)
        {
            Moto_Free_Event_Release();
            Moto_Voltage = Get_Moto_Value();
            if(Moto_Voltage>400)
            {
                LOG_D("Moto is Overload\r\n");
                rt_pin_write(MOTO_IN1,0);
                rt_pin_write(MOTO_IN2,0);
                MotoWorkFlag=0;
                jumpb();
                rt_timer_stop(Moto_Cycle_Timer);
                rt_event_send(&Moto_Event, Event_Moto_Free);
                ScreenTimerRefresh();
            }

        }
        if (rt_event_recv(&Moto_Event, (Event_Moto_Free | Event_Moto_Forward| Event_Moto_Back| Event_Moto_Cycle),RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0, &e) == RT_EOK)
        {
            LOG_D("recv event 0x%x\n", e);
            switch(e)
            {
                case Event_Moto_Free:
                    rt_pin_write(MOTO_IN1,0);
                    rt_pin_write(MOTO_IN2,0);
                    MotoWorkFlag=0;
                    break;
                case Event_Moto_Forward:
                    if(rt_pin_read(MOTO_RIGHT)==1)
                    {
                        rt_pin_write(MOTO_IN1,0);
                        rt_pin_write(MOTO_IN2,1);
                        MotoWorkFlag=1;
                    }
                    else
                    {
                        MotoWorkFlag=1;
                        LOG_D("Right Switch is Cover\r\n");
                    }
                    break;
                case Event_Moto_Back:
                    if(rt_pin_read(MOTO_LEFT)==1)
                    {
                        rt_pin_write(MOTO_IN1,1);
                        rt_pin_write(MOTO_IN2,0);
                        MotoWorkFlag=2;
                    }
                    break;
            }
        }
        rt_thread_mdelay(50);
    }
}
void Moto_TaskUp(void)
{
    Moto_Thread = rt_thread_create("Moto_Thread",Moto_Entry,RT_NULL,2048,5,10);
    if(Moto_Thread!=RT_NULL)
    {
        rt_thread_startup(Moto_Thread);
    }
}
MSH_CMD_EXPORT(Moto_TaskUp,Moto_TaskUp);
