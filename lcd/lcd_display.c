#include "rtthread.h"
#include "lkdGui.h"
#include "12864.h"
#include "DeviceInit.h"
#include "lcd_display.h"
#include "Flashwork.h"
#include "Adcwork.h"
#include "MotoControl.h"
#include "tdswork.h"

rt_thread_t lcd_task=RT_NULL;
rt_sem_t Jump_To_A = RT_NULL;
rt_sem_t Jump_To_B = RT_NULL;
rt_sem_t Jump_To_C = RT_NULL;

char *Manual="Manual";
char *Reminder="Reminder";
char *Automatic="Automatic";
char *Delta1="Delta Press";
char *Delta2="Delta Press(M)";
char *Delta3="Delta Press(A)";
char *Info="Info";
char *Down_arrow="}";
char *Back="Back       Select";
char *SingleBack="Back";
char *SingleYes="              Yes";
char *SingleSelect="           Select";
char *YesOrNo="No            Yes";
char *Delta_Pressure="Delta Pressure";
char *Water_Hardness="Water Hardness";
char *Backwash_Time="Backwash Time";
char *Version="Version";
char *Factory="Factory";
char *Language="Language";
char *Update_FW="Update FW";
char *Smart_Link="Smart Link";
char *TDS_MODE="TDS Verify";

static void UserMain1WinFun(void *param);
static void UserMain2WinFun(void *param);
static void UserMain3WinFun(void *param);
static void UserMain4WinFun(void *param);
static void UserMain5WinFun(void *param);
static void UserMain6WinFun(void *param);
static void UserMain7WinFun(void *param);
static void UserMain8WinFun(void *param);
static void UserMain9WinFun(void *param);
static void UserMain10WinFun(void *param);
static void UserMain11WinFun(void *param);
static void UserMain12WinFun(void *param);
static void UserMain13WinFun(void *param);
static void UserMain14WinFun(void *param);
static void UserMain15WinFun(void *param);
static void UserMain16WinFun(void *param);
static void UserMain17WinFun(void *param);
static void UserMain18WinFun(void *param);
static void UserMain19WinFun(void *param);
static void UserMain20WinFun(void *param);
static void UserMain21WinFun(void *param);
static void UserMain22WinFun(void *param);
static void UserMain23WinFun(void *param);
static void UserMain24WinFun(void *param);
static void UserMain25WinFun(void *param);

uint8_t Reminder_Week=0;
uint8_t Reminder_Day=0;
uint8_t Reminder_Enable=0;
uint8_t Automatic_Week=0;
uint8_t Automatic_Day=0;
uint8_t Automatic_Enable=0;
uint8_t Deltapress_Enable=0;
uint16_t Counter_Manual=0;
uint16_t Counter_Automatic=0;
uint16_t Counter_Deltapress=0;
uint16_t Counter_Error=0;
uint8_t Setting_Deltapress=0; //0:Report only  1:Automatic
uint16_t Setting_Hardness=0;
uint16_t Setting_Backwashtime=0;
uint8_t Setting_Language=0;

void GotValue(void)
{
        Reminder_Week = Flash_Get(1);
        Reminder_Day = Flash_Get(2);
        Reminder_Enable = Flash_Get(3);
        Automatic_Week = Flash_Get(4);
        Automatic_Day = Flash_Get(5);
        Automatic_Enable = Flash_Get(6);
        Deltapress_Enable = Flash_Get(7);
        Counter_Manual = Flash_Get(8);
        Counter_Automatic = Flash_Get(9);
        Counter_Deltapress = Flash_Get(10);
        Counter_Error = Flash_Get(11);
        Setting_Deltapress = Flash_Get(12);
        Setting_Hardness = Flash_Get(13);
        Setting_Backwashtime = Flash_Get(14);
        if(Setting_Backwashtime==0||Setting_Backwashtime>120)
        {
            Setting_Backwashtime = 30;
            Flash_Set(14,Setting_Backwashtime);
        }
        Setting_Language = Flash_Get(15);
}

extern rt_sem_t K0_Sem;
extern rt_sem_t K1_Sem;
extern rt_sem_t K2_Sem;
extern rt_sem_t K2_Long_Sem;

static rt_err_t K0_Status;
static rt_err_t K1_Status;
static rt_err_t K2_Status;
static rt_err_t K2_Long_Status;

static rt_err_t Jump_To_A_Status;
static rt_err_t Jump_To_B_Status;
static rt_err_t Jump_To_C_Status;
static uint8_t Jump_Flag=1;

lkdButton tButton[5];
lkdScroll tScroll[2];

char NowButtonId=0;
char NowSetting=0;
char NowWinid=0;
char FirstFlag[30]={0};
extern uint8_t LCD_Flag;
extern void defaultFontInit(void);
//以下为窗口参数配置
lkdWin userMain1Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    //.title = "Main Menu",
    .param = NULL,
    .WindowFunction = UserMain1WinFun,
        .firstflag = 0,
};
lkdWin userMain2Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Manual",
    .param = NULL,
    .WindowFunction = UserMain2WinFun,
        .firstflag = 0,
};
lkdWin userMain3Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    //.title = "Main Window",
    .param = NULL,
    .WindowFunction = UserMain3WinFun,
        .firstflag = 0,
};
lkdWin userMain4Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Reminder In",
    .param = NULL,
    .WindowFunction = UserMain4WinFun,
        .firstflag = 0,
};
lkdWin userMain5Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Automatic In",
    .param = NULL,
    .WindowFunction = UserMain5WinFun,
        .firstflag = 0,
};
lkdWin userMain6Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Delta Pressure",
    .param = NULL,
    .WindowFunction = UserMain6WinFun,
        .firstflag = 0,
};
lkdWin userMain7Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Info",
    .param = NULL,
    .WindowFunction = UserMain7WinFun,
        .firstflag = 0,
};
lkdWin userMain8Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Counter",
    .param = NULL,
    .WindowFunction = UserMain8WinFun,
        .firstflag = 0,
};
lkdWin userMain9Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Counter Reading",
    .param = NULL,
    .WindowFunction = UserMain9WinFun,
        .firstflag = 0,
};
lkdWin userMain10Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Counter Reading",
    .param = NULL,
    .WindowFunction = UserMain10WinFun,
        .firstflag = 0,
};
lkdWin userMain11Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Counter Reading",
    .param = NULL,
    .WindowFunction = UserMain11WinFun,
        .firstflag = 0,
};
lkdWin userMain12Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Counter Reading",
    .param = NULL,
    .WindowFunction = UserMain12WinFun,
        .firstflag = 0,
};
lkdWin userMain13Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Info",
    .param = NULL,
    .WindowFunction = UserMain13WinFun,
        .firstflag = 0,
};
lkdWin userMain14Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Settings",
    .param = NULL,
    .WindowFunction = UserMain14WinFun,
        .firstflag = 0,
};
lkdWin userMain15Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Delta Set",
    .param = NULL,
    .WindowFunction = UserMain15WinFun,
        .firstflag = 0,
};
lkdWin userMain16Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Hardness Set",
    .param = NULL,
    .WindowFunction = UserMain16WinFun,
        .firstflag = 0,
};
lkdWin userMain17Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Backwash Time",
    .param = NULL,
    .WindowFunction = UserMain17WinFun,
        .firstflag = 0,
};
lkdWin userMain18Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Version",
    .param = NULL,
    .WindowFunction = UserMain18WinFun,
        .firstflag = 0,
};
lkdWin userMain19Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Factory",
    .param = NULL,
    .WindowFunction = UserMain19WinFun,
        .firstflag = 0,
};
lkdWin userMain20Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Language",
    .param = NULL,
    .WindowFunction = UserMain20WinFun,
        .firstflag = 0,
};
lkdWin userMain21Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Password",
    .param = NULL,
    .WindowFunction = UserMain21WinFun,
        .firstflag = 0,
};
lkdWin userMain22Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "TDS Calibration",
    .param = NULL,
    .WindowFunction = UserMain22WinFun,
        .firstflag = 0,
};
lkdWin userMain23Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Cuttent Value",
    .param = NULL,
    .WindowFunction = UserMain23WinFun,
        .firstflag = 0,
};
lkdWin userMain24Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    .title = "Calibration Value",
    .param = NULL,
    .WindowFunction = UserMain24WinFun,
        .firstflag = 0,
};
lkdWin userMain25Win = {
    .x = 0,
    .y = 0,
    .wide = 128,
    .hight = 64,
    //.title = "Calibration Value",
    .param = NULL,
    .WindowFunction = UserMain25WinFun,
        .firstflag = 0,
};
void Lcd_Sem_init(void)
{
        Jump_To_A=rt_sem_create("LCD_JUMP_A", 0, RT_IPC_FLAG_FIFO);
        Jump_To_B=rt_sem_create("LCD_JUMP_B", 0, RT_IPC_FLAG_FIFO);
        Jump_To_C=rt_sem_create("LCD_JUMP_C", 0, RT_IPC_FLAG_FIFO);
}
//以下为显示启动以及测试部分
void userAppPortInit(void)
{
    /* 1. 初始化lcd */
        LcdInit();
    /* 2. 初始化用户字体 */
    defaultFontInit();

    /* 3. 初始化窗口,添加窗口 */
    GuiWinInit();
    GuiWinAdd(&userMain1Win);
        //GuiWinDisplay();

}
void userAppPortRun(void)
{
    /* 窗口调度管理 */
    GuiWinDisplay();
}
void test_drawpoint(void)
{
        userAppPortInit();
        GuiClearScreen(0);
        GuiDrawPoint(0,0,1);
        GuiDrawPoint(0,63,1);
        GuiDrawPoint(62,30,1);
        GuiDrawPoint(62,31,1);
        GuiDrawPoint(63,30,1);
        GuiDrawPoint(63,31,1);
        GuiDrawPoint(127,0,1);
        GuiDrawPoint(127,63,1);
}
MSH_CMD_EXPORT(test_drawpoint,test_drawpoint);
void jumpa(void)
{
        rt_sem_release(Jump_To_A);
}
MSH_CMD_EXPORT(jumpa,test_drawpoint);
void jumpb(void)
{
        Counter_Error++;
        Flash_Set(11,Counter_Error);
        rt_sem_release(Jump_To_B);
}
MSH_CMD_EXPORT(jumpb,test_drawpoint);
void jumpc(void)
{
        rt_sem_release(Jump_To_C);
}
MSH_CMD_EXPORT(jumpc,test_drawpoint);
uint8_t JumptoReminderFlag=0;
void JumptoReminder(void)
{
        JumptoReminderFlag=1;
}
void LcdtoReminder(void)
{
        if(FirstFlag[25]==0&&Jump_Flag==1)
        {
            for(uint8_t i=0;i<=25;i++)
            {
                FirstFlag[i]=0;
            }
            GuiClearScreen(0);
            GuiWinAdd(&userMain25Win);
            GuiWinDisplay();
        }
        else
        {
            FirstFlag[25]=0;
        }

}
void LcdtoBackwash(void)
{
    if(FirstFlag[3]==0)
    {
        for(uint8_t i=0;i<=25;i++)
        {
            FirstFlag[i]=0;
        }
        GuiClearScreen(0);
        GuiWinAdd(&userMain3Win);
        GuiWinDisplay();
    }
    else
    {
        FirstFlag[3]=0;
    }
    Moto_Cycle();
}
uint8_t JumptoAutomaticFlag=0;
void JumptoAutomatic(void)
{
    Counter_Automatic++;
    Flash_Set(9,Counter_Automatic);
    JumptoAutomaticFlag=1;
}
MSH_CMD_EXPORT(JumptoReminder,JumptoReminder);
void lcd_task_entry(void *parameter)
{
    GotValue();
    Lcd_Sem_init();
    userAppPortInit();
    LCD_Flag = 0;

    while(1)
        {
            userAppPortRun();
            Delta_press_work();
            if(LCD_Flag!=3)
            {
                GuiUpdateDisplayAll();
            }
            if(JumptoReminderFlag)
            {
                JumptoReminderFlag=0;
                LcdtoReminder();
                OpenLcdDisplay();
                ScreenTimerRefresh();
            }
            if(JumptoAutomaticFlag)
            {
                JumptoAutomaticFlag=0;
                LcdtoBackwash();
                OpenLcdDisplay();
//                ScreenTimerRefresh();
            }
            rt_thread_mdelay(10);
        }
}
void lcd(void)
{
        lcd_task=rt_thread_create("lcd_task",lcd_task_entry,RT_NULL,2048,20,10);
        if(lcd_task!=RT_NULL)rt_thread_startup(lcd_task);
}
MSH_CMD_EXPORT(lcd,lcd_task_init);

//以下为显示窗口部分
uint8_t Main1NextPage=0;
static void UserMain1WinFun(void *param)
{
     if(FirstFlag[1] == 0||Main1NextPage==1)
        {   //绘图等第一次进入窗口的相关执行动作
        NowButtonId=0;
        NowWinid = 1;
        //GuiUpdateDisplayAll();
        tScroll[0].max = 5;
        tScroll[0].x = 119;
        tScroll[0].y = 2;
        tScroll[0].hight = 48;
        tScroll[0].lump = 0;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 0;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].name = Manual;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 抬起状态 */

        tButton[1].x = 0;
        tButton[1].y = 11;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].name = Reminder;
        tButton[1].linesize = 60-!Reminder_Enable*60;
        tButton[1].flag = 0;/* 抬起状态 */

        tButton[2].x = 0;
        tButton[2].y = 24;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].name = Automatic;
        tButton[2].linesize = 70-!Automatic_Enable*70;
        tButton[2].flag = 0;/* 抬起状态 */

        tButton[3].x = 0;
        tButton[3].y = 36;
        tButton[3].high = 15;
        tButton[3].wide = 117;
        tButton[3].name = Delta_Pressure;
        tButton[3].linesize = 70-!Deltapress_Enable*70;
        if(Deltapress_Enable)
        {
            if(Setting_Deltapress)
            {
                tButton[3].wide = 110;
                tButton[3].linesize = 100-!Deltapress_Enable*100;
                tButton[3].name = Delta3;
            }
            else
            {
                tButton[3].wide = 110;
                tButton[3].linesize = 100-!Deltapress_Enable*100;
                tButton[3].name = Delta2;
            }
        }
        else
        {
            tButton[3].linesize = 0;
            tButton[3].wide = 117;
            tButton[3].name = Delta1;
        }
        tButton[3].flag = 0;/* 抬起状态 */

        tButton[4].x = 0;
        tButton[4].y = 51;
        tButton[4].wide = 128;
        tButton[4].high = 15;
        tButton[4].name = SingleSelect;
        tButton[4].flag = 1;/* 按下状态 */
        if(FirstFlag[1]  == 0&&Main1NextPage==0)
        {
            GuiButton(&tButton[4]);
        }
        GuiButton(&tButton[0]);
        GuiButton(&tButton[1]);
        GuiButton(&tButton[2]);
        GuiButton(&tButton[3]);
        Main1NextPage=0;
        FirstFlag[1] = 1;
     }
        else
        {
            K0_Status = rt_sem_take(K0_Sem, 0);
            K1_Status = rt_sem_take(K1_Sem, 0);
            K2_Status = rt_sem_take(K2_Sem, 0);
            K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
            if(K2_Long_Status==RT_EOK)
            {
                FirstFlag[1]=0;
                GuiClearScreen(0);
                GuiWinAdd(&userMain14Win);
                GuiWinDisplay();
            }
            if(K1_Status==RT_EOK)
            {
                switch(NowButtonId)
                {
                    case 3:
                        tButton[0].name=Reminder;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 60-!Reminder_Enable*60;
                        tButton[1].name=Automatic;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 70-!Automatic_Enable*70;
                        if(Deltapress_Enable)
                        {
                            if(Setting_Deltapress)
                            {
                                tButton[2].wide = 110;
                                tButton[2].linesize = 100-!Deltapress_Enable*100;
                                tButton[2].name = Delta3;
                            }
                            else
                            {
                                tButton[2].wide = 110;
                                tButton[2].linesize = 100-!Deltapress_Enable*100;
                                tButton[2].name = Delta2;
                            }

                        }
                        else
                        {
                            tButton[2].linesize = 0;
                            tButton[2].wide = 117;
                            tButton[2].name = Delta1;
                        }
                        tButton[3].name=Info;
                        GuiButton(&tButton[3]);
                        GuiButton(&tButton[2]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[0]);
                        NowButtonId++;
                        break;
                    case 4:
                        NowButtonId++;
                        Main1NextPage=1;
                        FirstFlag[1]  = 0;
                        break;
                    default:
                        tButton[NowButtonId].flag=0;
                        GuiButton(&tButton[NowButtonId]);
                        NowButtonId++;
                        tButton[NowButtonId].flag=1;
                        GuiButton(&tButton[NowButtonId]);
                        break;
                }
                    tScroll[0].lump = NowButtonId;/* 进度快控制 */
                    GuiVScroll(&tScroll[0]);/* 垂直进度条 */
            }
            if(K2_Status==RT_EOK)
            {
                FirstFlag[1]=0;
                GuiClearScreen(0);
                switch(NowButtonId)
                {
                    case 0:GuiWinAdd(&userMain2Win);break;//Manual
                    case 1:GuiWinAdd(&userMain4Win);break;//Reminder
                    case 2:GuiWinAdd(&userMain5Win);break;//Automatic
                    case 3:GuiWinAdd(&userMain6Win);break;//Delta Press
                    case 4:GuiWinAdd(&userMain7Win);break;//Delta Press
                }
                GuiWinDisplay();
            }
        }
}
static void UserMain2WinFun(void *param)
{
     if(FirstFlag[2] == 0)
         {
                FirstFlag[2] = 1;
                NowWinid=2;
                //GuiUpdateDisplayAll();
                GuiRowText(40,15,57,0,"Perform");
                GuiRowText(40,27,57,0,"Manual");
                GuiRowText(40,39,57,0,"Backwash");
                tButton[0].x = 0;
                tButton[0].y = 50;
                tButton[0].wide = 128;
                tButton[0].high = 15;
                tButton[0].name = YesOrNo;
                tButton[0].linesize = 0;
                tButton[0].flag = 1;/* 按下状态 */
                GuiButton(&tButton[0]);
     }
         else
         {
                K0_Status = rt_sem_take(K0_Sem, 0);
                K1_Status = rt_sem_take(K1_Sem, 0);
                K2_Status = rt_sem_take(K2_Sem, 0);
                K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
                if(K2_Long_Status==RT_EOK)
                {

                }
                if(K0_Status==RT_EOK)
                {
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    GuiWinDisplay();
                    FirstFlag[2]=0;
                }
                if(K1_Status==RT_EOK)
                {
                }
                if(K2_Status==RT_EOK)
                {
                    FirstFlag[2]=0;
                    Counter_Manual++;
                    Flash_Set(8,Counter_Manual);
                    Moto_Cycle();
                    GuiClearScreen(0);
                    GuiWinAdd(&userMain3Win);
                    GuiWinDisplay();
                }
         }
}
rt_timer_t SemJump_Timer=RT_NULL;
void K2_Setjump_Sem_Release(void *parameter)
{
         rt_sem_release(K2_Sem);
}
void SemJump (void)
{
        SemJump_Timer = rt_timer_create("SemJump_Timer",K2_Setjump_Sem_Release,RT_NULL,4000,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
        rt_timer_start(SemJump_Timer);
}
static void UserMain3WinFun(void *param)
{
     if(FirstFlag[3] == 0)
         {
            FirstFlag[3] = 1;
            NowWinid = 3;
            Jump_Flag=0;
            GuiClearScreen(0);
            //GuiUpdateDisplayAll();
            GuiRowText(35,15,57,0,"Backwash");
            GuiRowText(30,27,70,0,"In progress");
            GuiRowText(25,39,85,0,"-->-->-->-->");
            GuiRowText(106,56,30,0,"   ");
     }
         else
         {
                Jump_To_A_Status = rt_sem_take(Jump_To_A, 0);
                Jump_To_B_Status = rt_sem_take(Jump_To_B, 0);
                Jump_To_C_Status = rt_sem_take(Jump_To_C, 0);
                K0_Status = rt_sem_take(K0_Sem, 0);
                K1_Status = rt_sem_take(K1_Sem, 0);
                K2_Status = rt_sem_take(K2_Sem, 0);
                K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
                if(K2_Long_Status==RT_EOK)
                {

                }
                if(Jump_To_A_Status==RT_EOK)
                {
                    GuiClearScreen(0);
                    //GuiUpdateDisplayAll();
                    GuiRowText(0,1,125,0,"Increased water");
                    GuiRowText(0,13,125,0,"hardness");
                    GuiRowText(0,25,127,0,"The use of a water");
                    GuiRowText(0,37,125,0,"soft system");
                    GuiRowText(0,49,125,0,"is recommended");
                    GuiRowText(106,56,30,0,"Yes");
                    Jump_Flag=1;
                }
                if(Jump_To_B_Status==RT_EOK)
                {
                    GuiClearScreen(0);
                    //GuiUpdateDisplayAll();
                    GuiRowText(0,30,127,0,"Valve is not closed");
                    GuiRowText(106,56,30,0,"Yes");
                    Red_Light();
                    Jump_Flag=1;
                }
                if(Jump_To_C_Status==RT_EOK)
                {
                    GuiClearScreen(0);
                    //GuiUpdateDisplayAll();
                    GuiRowText(35,15,57,0,"Backwash");
                    GuiRowText(40,27,70,0,"Finish");
                    GuiRowText(106,56,30,0,"Yes");
                    Jump_Flag=1;
                    SemJump();
                }
                if(K2_Status==RT_EOK&&Jump_Flag==1)
                {
                    FirstFlag[3]=0;
                    GuiClearScreen(0);
                    GuiWinInit();
                    GuiWinAdd(&userMain1Win);
                    GuiWinDisplay();
                    Red_Off();
                    Green_Off();
                }
         }
}


uint8_t Reminder_Week_Temp;
uint8_t Reminder_Day_Temp;
uint8_t Reminder_Enable_Temp;
extern uint32_t RTC_Reminder_Time ;
static void UserMain4WinFun(void *param)
{
    uint8_t weekstring[]={"00"};
    uint8_t daystring[]={"00"};
    if(FirstFlag[4] == 0)
    {
            NowWinid = 4;
            //GuiUpdateDisplayAll();
            Reminder_Week_Temp=Reminder_Week;
            Reminder_Day_Temp=Reminder_Day;
            Reminder_Enable_Temp=Reminder_Enable;
            //GuiUpdateDisplayAll();
            FirstFlag[4] = 1;
            GuiRowText(35,17,70,0,"Weeks:");
            sprintf(weekstring,"%02d",Reminder_Week);
            GuiRowText(77,17,70,0,weekstring);
            GuiRowText(35,17,70,0,"Weeks:");
            tButton[0].x = 0;
            tButton[0].y = 50;
            tButton[0].wide = 128;
            tButton[0].high = 15;
            tButton[0].name = Back;
            tButton[0].linesize = 0;
            tButton[0].flag = 1;/* 按下状态 */
            GuiButton(&tButton[0]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            NowSetting=0;
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[4]=0;
        }
        if(K1_Status==RT_EOK)
        {
            switch(NowSetting)
            {
                case 0:
                    if(Reminder_Week_Temp++>=51)Reminder_Week_Temp=0;
                    sprintf(weekstring,"%02d",Reminder_Week_Temp);
                    GuiRowText(77,17,70,0,weekstring);
                    break;
                case 1:
                    if(Reminder_Day_Temp++>=30)Reminder_Day_Temp=0;
                    sprintf(daystring,"%02d",Reminder_Day_Temp);
                    GuiRowText(72,29,70,0,daystring);
                    break;
                case 2:
                    Reminder_Enable_Temp=!Reminder_Enable_Temp;
                    if(Reminder_Enable_Temp){GuiRowText(37,41,70,0,"          ");GuiRowText(37,41,70,0,"Enable");}
                    else{GuiRowText(37,41,70,0,"          ");GuiRowText(37,41,70,0,"Disable");}
                    break;
            }
        }
        if(K2_Status==RT_EOK)
        {
                switch(NowSetting)
                {
                    case 0:
                        GuiRowText(37,29,70,0,"Days:");
                        sprintf(daystring,"%02d",Reminder_Day);
                        GuiRowText(72,29,70,0,daystring);
                        NowSetting++;
                        break;
                    case 1:
                        NowSetting++;
                        if(Reminder_Enable_Temp){GuiRowText(37,41,70,0,"       ");GuiRowText(37,41,70,0,"Enable");}
                        else{GuiRowText(37,41,70,0,"       ");GuiRowText(37,41,70,0,"Disable");}
                        break;
                    case 2:
                        FirstFlag[4]=0;
                        NowSetting=0;
                        if(Reminder_Week_Temp==0&&Reminder_Day_Temp==0)Reminder_Day_Temp=1;
                        Reminder_Week=Reminder_Week_Temp;
                        Flash_Set(1,Reminder_Week_Temp);
                        Reminder_Day=Reminder_Day_Temp;
                        Flash_Set(2,Reminder_Day_Temp);
                        RTC_Reminder_Time=0;
                        rt_kprintf("RTC_Reminder_Time Change to 0\r\n");
                        Reminder_Enable=Reminder_Enable_Temp;
                        Flash_Set(3,Reminder_Enable_Temp);
                        GuiClearScreen(0);
                        GuiWinDeleteTop();
                        GuiWinDisplay();
                        break;
                }

        }
    }
}
uint8_t Automatic_Week_Temp;
uint8_t Automatic_Day_Temp;
uint8_t Automatic_Enable_Temp;
extern uint32_t RTC_Automatic_Time ;
static void UserMain5WinFun(void *param)
{
    uint8_t weekstring[]={"00"};
    uint8_t daystring[]={"00"};
    if(FirstFlag[5] == 0)
    {
        NowWinid = 5;
        Automatic_Week_Temp=0;
        Automatic_Day_Temp=0;
        Automatic_Enable_Temp=Automatic_Enable;
        //GuiUpdateDisplayAll();
        FirstFlag[5] = 1;
        GuiRowText(35,17,70,0,"Weeks:");
        sprintf(weekstring,"%02d",Automatic_Week);
        GuiRowText(77,17,70,0,weekstring);
        GuiRowText(35,17,70,0,"Weeks:");
        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = Back;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[5]=0;
            NowSetting=0;
        }
        if(K1_Status==RT_EOK)
        {
            switch(NowSetting)
            {
                case 0:
                    if(Automatic_Week_Temp++>=51)Automatic_Week_Temp=0;
                    sprintf(weekstring,"%02d",Automatic_Week_Temp);
                    GuiRowText(77,17,70,0,weekstring);
                    break;
                case 1:
                    if(Automatic_Day_Temp++>=30)Automatic_Day_Temp=0;
                    sprintf(daystring,"%02d",Automatic_Day_Temp);
                    GuiRowText(72,29,70,0,daystring);
                    break;
                case 2:
                    Automatic_Enable_Temp=!Automatic_Enable_Temp;
                    if(Automatic_Enable_Temp){GuiRowText(37,41,70,0,"          ");GuiRowText(37,41,70,0,"Enable");}
                    else{GuiRowText(37,41,70,0,"          ");GuiRowText(37,41,70,0,"Disable");}
                    break;
            }
        }
        if(K2_Status==RT_EOK)
        {
                switch(NowSetting)
                {
                    case 0:
                        GuiRowText(37,29,70,0,"Days:");
                        sprintf(daystring,"%02d",Automatic_Day);
                        GuiRowText(72,29,70,0,daystring);
                        NowSetting++;
                        break;
                    case 1:
                        NowSetting++;
                        if(Automatic_Enable_Temp){GuiRowText(37,41,70,0,"          ");GuiRowText(37,41,70,0,"Enable");}
                        else{GuiRowText(37,41,70,0,"          ");GuiRowText(37,41,70,0,"Disable");}
                        break;
                    case 2:
                        FirstFlag[5]=0;
                        NowSetting=0;
                        if(Automatic_Week_Temp==0&&Automatic_Day_Temp==0)Automatic_Day_Temp=1;
                        Automatic_Week=Automatic_Week_Temp;
                        Flash_Set(4,Automatic_Week_Temp);
                        Automatic_Day=Automatic_Day_Temp;
                        Flash_Set(5,Automatic_Day_Temp);
                        RTC_Automatic_Time=0;
                        rt_kprintf("RTC_Automatic_Time Change to 0\r\n");
                        Automatic_Enable=Automatic_Enable_Temp;
                        Flash_Set(6,Automatic_Enable_Temp);
                        GuiClearScreen(0);
                        GuiWinDeleteTop();
                        GuiWinDisplay();
                        break;
                }
        }
    }
}
uint8_t Deltapress_Enable_Temp=0;
static void UserMain6WinFun(void *param)
{
    if(FirstFlag[6] == 0)
    {
        NowWinid = 6;
        FirstFlag[6] = 1;
        //GuiUpdateDisplayAll();
        Deltapress_Enable_Temp=Deltapress_Enable;
        GuiRowText(27,15,100,0,"Backwash is");

        tButton[0].x = 35;
        tButton[0].y = 23;
        tButton[0].wide = 55;
        tButton[0].high = 15;
        tButton[0].name = "Disable";
        tButton[0].linesize = 0;
        tButton[0].flag = !Deltapress_Enable_Temp;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 38;
        tButton[1].y = 35;
        tButton[1].wide = 48;
        tButton[1].high = 15;
        tButton[1].name = "Enable";
        tButton[1].linesize = 0;
        tButton[1].flag = Deltapress_Enable_Temp;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = YesOrNo;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[6]=0;
        }
        if(K1_Status==RT_EOK)
        {
            Deltapress_Enable_Temp=!Deltapress_Enable_Temp;
            tButton[0].flag = !Deltapress_Enable_Temp;
            tButton[1].flag = Deltapress_Enable_Temp;
            GuiButton(&tButton[0]);
            GuiButton(&tButton[1]);
        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[6]=0;
            Deltapress_Enable=Deltapress_Enable_Temp;
            Flash_Set(7,Deltapress_Enable_Temp);
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
        }
    }
}
static void UserMain7WinFun(void *param)
{
    if(FirstFlag[7] == 0)
    {
        FirstFlag[7] = 1;
        NowWinid = 7;
        NowButtonId=0;
        //GuiUpdateDisplayAll();

        tScroll[0].max = 3;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 0;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Backwash Counter";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].linesize = 0;
        tButton[1].name = "Error Counter";
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].linesize = 0;
        tButton[2].name = "Battery States";
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = YesOrNo;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[7]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==3){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            tScroll[0].lump = NowButtonId;/* 进度快控制 */
            GuiVScroll(&tScroll[0]);/* 垂直进度条 */
        }
        if(K2_Status==RT_EOK)
        {
                FirstFlag[7]=0;
                GuiClearScreen(0);
                switch(NowButtonId)
                {
                    GuiClearScreen(0);
                    case 0:GuiWinAdd(&userMain8Win);break;//Manual
                    case 1:GuiWinAdd(&userMain12Win);break;//Error
                    case 2:GuiWinAdd(&userMain13Win);break;//Battery
                }
                GuiWinDisplay();
        }
    }
}
static void UserMain8WinFun(void *param)
{
    if(FirstFlag[8] == 0)
    {
        FirstFlag[8] = 1;
        NowWinid = 8;
        NowButtonId=0;
        //GuiUpdateDisplayAll();

        tScroll[0].max = 3;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 0;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Manual";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].linesize = 0;
        tButton[1].name = "Automatic";
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].linesize = 0;
        tButton[2].name = "Delta Pressure";
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[8]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==3){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            tScroll[0].lump = NowButtonId;/* 进度快控制 */
            GuiVScroll(&tScroll[0]);/* 垂直进度条 */
        }
        if(K2_Status==RT_EOK)
        {
                FirstFlag[8]=0;
                GuiClearScreen(0);
                switch(NowButtonId)
                {
                    GuiClearScreen(0);
                    case 0:GuiWinAdd(&userMain9Win);break;//Manual
                    case 1:GuiWinAdd(&userMain10Win);break;//Automatic
                    case 2:GuiWinAdd(&userMain11Win);break;//Delta
                }
                GuiWinDisplay();
        }
    }
}
static void UserMain9WinFun(void *param)
{
    uint8_t ManualString[4];
    if(FirstFlag[9] == 0)
    {
        FirstFlag[9] = 1;
        NowWinid = 9;
        NowButtonId=0;
        //GuiUpdateDisplayAll();

        tButton[0].x = 25;
        tButton[0].y = 13;
        tButton[0].wide = 85;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Manual:";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        sprintf(ManualString,"%02d",Counter_Manual);
        GuiExchangeColor();
        GuiRowText(81,17,30,0,ManualString);
        GuiExchangeColor();

        tButton[1].x = 40;
        tButton[1].y = 25;
        tButton[1].wide = 50;
        tButton[1].high = 15;
        tButton[1].name = "Reset?";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);


        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = Back;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[9]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==2){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId==1)
            {
                sprintf(ManualString,"%02d",Counter_Manual);
                GuiRowText(81,17,30,0,ManualString);

            }
            else
            {
                GuiExchangeColor();
                sprintf(ManualString,"%02d",Counter_Manual);
                GuiRowText(81,17,30,0,ManualString);
                GuiExchangeColor();
            }

        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[9]=0;
            switch(NowButtonId)
            {
                GuiClearScreen(0);
                case 0:break;
                case 1:
                    Counter_Manual=0;
                    Flash_Set(8,0);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
            }
            GuiWinDisplay();
        }
    }
}
static void UserMain10WinFun(void *param)
{
    uint8_t AutomaticString[4];
    if(FirstFlag[10] == 0)
    {
        FirstFlag[10] = 1;
        NowWinid = 10;
        NowButtonId=0;
        //GuiUpdateDisplayAll();

        tButton[0].x = 18;
        tButton[0].y = 13;
        tButton[0].wide = 90;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Automatic:";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        sprintf(AutomaticString,"%02d",Counter_Automatic);
        GuiExchangeColor();
        GuiRowText(92,17,30,0,AutomaticString);
        GuiExchangeColor();

        tButton[1].x = 40;
        tButton[1].y = 25;
        tButton[1].wide = 50;
        tButton[1].high = 15;
        tButton[1].name = "Reset?";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);


        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = Back;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[10]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==2){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId==1)
            {
                sprintf(AutomaticString,"%02d",Counter_Automatic);
                GuiRowText(92,17,30,0,AutomaticString);

            }
            else
            {
                GuiExchangeColor();
                sprintf(AutomaticString,"%02d",Counter_Automatic);
                GuiRowText(92,17,30,0,AutomaticString);
                GuiExchangeColor();
            }

        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[10]=0;
            switch(NowButtonId)
            {
                GuiClearScreen(0);
                case 0:break;
                case 1:
                    Counter_Automatic=0;
                    Flash_Set(9,0);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
            }
            GuiWinDisplay();
        }
    }
}
static void UserMain11WinFun(void *param)
{
    uint8_t DeltaString[4];
    if(FirstFlag[11] == 0)
    {
        FirstFlag[11] = 1;
        NowWinid = 11;
        NowButtonId=0;
        //GuiUpdateDisplayAll();

        tButton[0].x = 0;
        tButton[0].y = 13;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Delta Pressure:";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        sprintf(DeltaString,"%02d",Counter_Deltapress);
        GuiExchangeColor();
        GuiRowText(110,17,30,0,DeltaString);
        GuiExchangeColor();

        tButton[1].x = 40;
        tButton[1].y = 25;
        tButton[1].wide = 50;
        tButton[1].high = 15;
        tButton[1].name = "Reset?";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);


        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = Back;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[11]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==2){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId==1)
            {
                sprintf(DeltaString,"%02d",Counter_Deltapress);
                GuiRowText(110,17,30,0,DeltaString);

            }
            else
            {
                GuiExchangeColor();
                sprintf(DeltaString,"%02d",Counter_Deltapress);
                GuiRowText(110,17,30,0,DeltaString);
                GuiExchangeColor();
            }

        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[11]=0;
            switch(NowButtonId)
            {
                GuiClearScreen(0);
                case 0:break;
                case 1:
                    Counter_Deltapress=0;
                    Flash_Set(10,0);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
            }
            GuiWinDisplay();
        }
    }
}
static void UserMain12WinFun(void *param)
{
    uint8_t ErrorString[6];
    if(FirstFlag[12] == 0)
    {
        FirstFlag[12] = 1;
        NowWinid = 12;
        NowButtonId=0;
        //GuiUpdateDisplayAll();

        tButton[0].x = 0;
        tButton[0].y = 13;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Error Counter:";
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        sprintf(ErrorString,"%02d",Counter_Error);
        GuiExchangeColor();
        GuiRowText(105,17,30,0,ErrorString);
        GuiExchangeColor();

        tButton[1].x = 40;
        tButton[1].y = 25;
        tButton[1].wide = 50;
        tButton[1].high = 15;
        tButton[1].name = "Reset?";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);


        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = Back;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[12]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==2){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
            if(NowButtonId==1)
            {
                sprintf(ErrorString,"%02d",Counter_Error);
                GuiRowText(110,17,30,0,ErrorString);

            }
            else
            {
                GuiExchangeColor();
                sprintf(ErrorString,"%02d",Counter_Error);
                GuiRowText(110,17,30,0,ErrorString);
                GuiExchangeColor();
            }

        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[12]=0;
            switch(NowButtonId)
            {
                GuiClearScreen(0);
                case 0:break;
                case 1:
                    Counter_Error=0;
                    Flash_Set(11,0);
                    GuiClearScreen(0);
                    GuiWinDeleteTop();
                    break;
            }
            GuiWinDisplay();
        }
    }
}
static void UserMain13WinFun(void *param)
{
    if(FirstFlag[13] == 0)
    {
        FirstFlag[13] = 1;
        NowWinid = 13;
        //GuiUpdateDisplayAll();

        tButton[0].x = 13;
        tButton[0].y = 13;
        tButton[0].wide = 115;
        tButton[0].high = 15;
        tButton[0].linesize = 0;
        tButton[0].name = "Battery Status";
        tButton[0].flag = 0;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 52;
        tButton[1].y = 25;
        tButton[1].wide = 25;
        tButton[1].high = 15;
        if(Get_Bat_Level()){tButton[1].name = "OK";}
        else {tButton[1].name = "LOW";}
        tButton[1].linesize = 0;
        tButton[1].flag = 1;/* 按下状态 */
        GuiButton(&tButton[1]);


        tButton[2].x = 0;
        tButton[2].y = 50;
        tButton[2].wide = 128;
        tButton[2].high = 15;
        tButton[2].name = SingleBack;
        tButton[2].linesize = 0;
        tButton[2].flag = 1;/* 按下状态 */
        GuiButton(&tButton[2]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[13]=0;
        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {

        }
    }
}
static void UserMain14WinFun(void *param)
{
        if(FirstFlag[14]  == 0)
        {   //绘图等第一次进入窗口的相关执行动作
        NowButtonId=0;
        NowWinid = 14;
        //GuiUpdateDisplayAll();

        tScroll[0].max = 9;
        tScroll[0].x = 119;
        tScroll[0].y = 13;
        tScroll[0].hight = 38;
        tScroll[0].lump = 0;/* 进度快控制 */
        GuiVScroll(&tScroll[0]);/* 垂直进度条 */

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 117;
        tButton[0].high = 15;
        tButton[0].name = Delta_Pressure;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 抬起状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 117;
        tButton[1].high = 15;
        tButton[1].name = Water_Hardness;
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[2].x = 0;
        tButton[2].y = 37;
        tButton[2].wide = 117;
        tButton[2].high = 15;
        tButton[2].name = Backwash_Time;
        tButton[2].linesize = 0;
        tButton[2].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[2]);

        tButton[3].x = 0;
        tButton[3].y = 51;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);

        //GuiRowText(119,40,10,0,Down_arrow);

        FirstFlag[14] = 1;
     }
        else
        {
            K0_Status = rt_sem_take(K0_Sem, 0);
            K1_Status = rt_sem_take(K1_Sem, 0);
            K2_Status = rt_sem_take(K2_Sem, 0);
            K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
            if(K2_Long_Status==RT_EOK)
            {

            }
            if(K0_Status==RT_EOK)
            {
                FirstFlag[14]=0;
                GuiClearScreen(0);
                GuiWinDeleteTop();
                GuiWinDisplay();
            }
            if(K1_Status==RT_EOK)
            {
                switch(NowButtonId)
                {
                    case 2:
                        tButton[0].name=Water_Hardness;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 0;
                        tButton[1].name=Backwash_Time;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 0;
                        tButton[2].wide = 110;
                        tButton[2].linesize = 0;
                        tButton[2].name = Version;
                        GuiButton(&tButton[0]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[2]);
                        NowButtonId++;
                        break;
                    case 3:
                        tButton[0].name=Backwash_Time;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 0;
                        tButton[1].name=Version;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 0;
                        tButton[2].wide = 110;
                        tButton[2].linesize = 0;
                        tButton[2].name = Factory;
                        GuiButton(&tButton[0]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[2]);
                        NowButtonId++;
                        break;
                    case 4:
                        tButton[0].name=Version;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 0;
                        tButton[1].name=Factory;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 0;
                        tButton[2].wide = 110;
                        tButton[2].linesize = 0;
                        tButton[2].name = Language;
                        GuiButton(&tButton[0]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[2]);
                        NowButtonId++;
                        break;
                    case 5:
                        tButton[0].name=Factory;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 0;
                        tButton[1].name=Language;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 0;
                        tButton[2].wide = 110;
                        tButton[2].linesize = 0;
                        tButton[2].name = Update_FW;
                        GuiButton(&tButton[0]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[2]);
                        NowButtonId++;
                        break;
                    case 6:
                        tButton[0].name=Language;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 0;
                        tButton[1].name=Update_FW;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 0;
                        tButton[2].wide = 110;
                        tButton[2].linesize = 0;
                        tButton[2].name = Smart_Link;
                        GuiButton(&tButton[0]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[2]);
                        NowButtonId++;
                        break;
                    case 7:
                        tButton[0].name=Update_FW;
                        tButton[0].wide = 110;
                        tButton[0].linesize = 0;
                        tButton[1].name=Smart_Link;
                        tButton[1].wide = 110;
                        tButton[1].linesize = 0;
                        tButton[2].wide = 110;
                        tButton[2].linesize = 0;
                        tButton[2].name = TDS_MODE;
                        GuiButton(&tButton[0]);
                        GuiButton(&tButton[1]);
                        GuiButton(&tButton[2]);
                        NowButtonId++;
                        break;
                    case 8:
                        NowButtonId++;
                        FirstFlag[14] = 0;
                        break;

                    default:
                        tButton[NowButtonId].flag=0;
                        GuiButton(&tButton[NowButtonId]);
                        NowButtonId++;
                        tButton[NowButtonId].flag=1;
                        GuiButton(&tButton[NowButtonId]);
                        break;
                }
                tScroll[0].lump = NowButtonId;/* 进度快控制 */
                GuiVScroll(&tScroll[0]);/* 垂直进度条 */
            }
            if(K2_Status==RT_EOK)
            {
                FirstFlag[14]=0;
                GuiClearScreen(0);
                switch(NowButtonId)
                {
                    GuiClearScreen(0);
                    case 0:GuiWinAdd(&userMain15Win);break;//Delta Set
                    case 1:GuiWinAdd(&userMain16Win);break;//Hardness
                    case 2:GuiWinAdd(&userMain17Win);break;//Backwash Time
                    case 3:GuiWinAdd(&userMain18Win);break;//Version
                    case 4:GuiWinAdd(&userMain19Win);break;//Factory
                    case 5:GuiWinAdd(&userMain20Win);break;//Delta Press
                    case 8:GuiWinAdd(&userMain21Win);break;//Password
                    default:break;
                }
                GuiWinDisplay();
            }
        }
}
uint8_t Set_Delta_Temp;
static void UserMain15WinFun(void *param)
{
    if(FirstFlag[15] == 0)
    {
        FirstFlag[15] = 1;
        Set_Delta_Temp=Setting_Deltapress;
        NowWinid = 15;
        //GuiUpdateDisplayAll();

        tButton[0].x = 20;
        tButton[0].y = 12;
        tButton[0].wide = 85;
        tButton[0].high = 15;
        tButton[0].name = "Report Only";
        tButton[0].linesize = 100 - Set_Delta_Temp*100;
        tButton[0].flag = !Set_Delta_Temp;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 27;
        tButton[1].y = 24;
        tButton[1].wide = 72;
        tButton[1].high = 15;
        tButton[1].name = "Automatic";
        tButton[1].linesize = 94 - !Set_Delta_Temp*94;
        tButton[1].flag = Set_Delta_Temp;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[15]=0;
        }
        if(K1_Status==RT_EOK)
        {
            Set_Delta_Temp=!Set_Delta_Temp;
            tButton[0].flag = !Set_Delta_Temp;
            GuiButton(&tButton[0]);
            tButton[1].flag = Set_Delta_Temp;
            GuiButton(&tButton[1]);

        }
        if(K2_Status==RT_EOK)
        {
            Setting_Deltapress=Set_Delta_Temp;
            Flash_Set(12,Set_Delta_Temp);
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[15]=0;
        }
    }
}
uint16_t Set_Hardness_Temp;
static void UserMain16WinFun(void *param)
{
    uint8_t HardnessString[10];
    if(FirstFlag[16] == 0)
    {
        FirstFlag[16] = 1;
        Set_Hardness_Temp=0;
        NowWinid = 16;
        //GuiUpdateDisplayAll();

        GuiRowText(13,30,80,0,"Hardness:");
        sprintf(HardnessString,"%04d",Setting_Hardness);
        GuiRowText(76,30,80,0,HardnessString);
        GuiRowText(106,30,80,0,"uS");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = YesOrNo;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[16]=0;
        }
        if(K1_Status==RT_EOK)
        {
            Set_Hardness_Temp+=10;
            sprintf(HardnessString,"%04d",Set_Hardness_Temp);
            GuiRowText(76,30,80,0,HardnessString);
        }
        if(K2_Status==RT_EOK)
        {
            Setting_Hardness=Set_Hardness_Temp;
            Flash_Set(13,Set_Hardness_Temp);
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[16]=0;
        }
    }
}
uint16_t Set_Backwash_Temp;
static void UserMain17WinFun(void *param)
{
    uint8_t BackwashString[6];
    if(FirstFlag[17] == 0)
    {
        FirstFlag[17] = 1;
        Set_Backwash_Temp=30;
        NowWinid = 17;
        //GuiUpdateDisplayAll();

        GuiRowText(33,30,80,0,"Time:");
        sprintf(BackwashString,"%03d",Setting_Backwashtime);
        GuiRowText(70,30,80,0,BackwashString);
        GuiRowText(93,30,80,0,"S");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = YesOrNo;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[17]=0;
        }
        if(K1_Status==RT_EOK)
        {
            if(Set_Backwash_Temp<120)Set_Backwash_Temp++;
            else Set_Backwash_Temp=30;
            sprintf(BackwashString,"%03d",Set_Backwash_Temp);
            GuiRowText(70,30,80,0,BackwashString);
        }
        if(K2_Status==RT_EOK)
        {
            Setting_Backwashtime=Set_Backwash_Temp;
            Flash_Set(14,Set_Backwash_Temp);
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[17]=0;
        }
    }
}
static void UserMain18WinFun(void *param)
{
    if(FirstFlag[18] == 0)
    {
        FirstFlag[18] = 1;
        NowWinid = 18;
        //GuiUpdateDisplayAll();

        GuiRowText(19,30,80,0,"SYR BFC:");
        GuiRowText(76,30,80,0,"0.0.1");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = SingleBack;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[18]=0;
        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {

        }
    }
}
static void UserMain19WinFun(void *param)
{
    if(FirstFlag[19] == 0)
    {
        FirstFlag[19] = 1;
        NowWinid = 19;
        //GuiUpdateDisplayAll();

        GuiRowText(25,15,85,0,"Reset BFC to");
        GuiRowText(35,27,80,0,"Factory");
        GuiRowText(35,39,80,0,"Setting?");

        tButton[0].x = 0;
        tButton[0].y = 50;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = YesOrNo;
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[19]=0;
        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {
            Flash_Factory();
            rt_hw_cpu_reset();
        }
    }
}
uint8_t Set_Language_Temp;
static void UserMain20WinFun(void *param)
{
    if(FirstFlag[20] == 0)
    {
        FirstFlag[20] = 1;
        Set_Language_Temp=Setting_Language;
        NowWinid = 20;
        //GuiUpdateDisplayAll();

        tButton[0].x = 37;
        tButton[0].y = 12;
        tButton[0].wide = 56;
        tButton[0].high = 15;
        tButton[0].name = "English";
        tButton[0].linesize = 90 - Set_Language_Temp*90;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 41;
        tButton[1].y = 24;
        tButton[1].wide = 48;
        tButton[1].high = 15;
        tButton[1].name = "Deutsh";
        tButton[1].linesize = 85 - !Set_Language_Temp*85;
        tButton[1].flag = 0;/* 抬起状态 */
        GuiButton(&tButton[1]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[20]=0;
        }
        if(K1_Status==RT_EOK)
        {
            Set_Language_Temp=!Set_Language_Temp;
            tButton[0].flag = !Set_Language_Temp;
            GuiButton(&tButton[0]);
            tButton[1].flag = Set_Language_Temp;
            GuiButton(&tButton[1]);

        }
        if(K2_Status==RT_EOK)
        {
            Setting_Language=Set_Language_Temp;
            Flash_Set(15,Set_Language_Temp);
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[20]=0;
        }
    }
}
uint8_t Tds_Password=3;
uint8_t Tds_Password_Temp;
static void UserMain21WinFun(void *param)//password
{
    uint8_t Tds_PasswordString[4];
    if(FirstFlag[21] == 0)
    {
        FirstFlag[21] = 1;
        Tds_Password_Temp=0;
        NowWinid = 21;
        //GuiUpdateDisplayAll();

        GuiRowText(15,20,100,0,"Input Password");
        sprintf(Tds_PasswordString,"%02d",Tds_Password_Temp);
        GuiRowText(57,32,85,0,Tds_PasswordString);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[21]=0;
        }
        if(K1_Status==RT_EOK)
        {
            Tds_Password_Temp++;
            sprintf(Tds_PasswordString,"%02d",Tds_Password_Temp);
            GuiRowText(57,32,85,0,Tds_PasswordString);
        }
        if(K2_Status==RT_EOK)
        {
            GuiClearScreen(0);
            if(Tds_Password_Temp==Tds_Password)
            {
                GuiWinAdd(&userMain22Win);
            }
            else
            {
                GuiWinDeleteTop();
            }
            GuiWinDisplay();
            FirstFlag[21]=0;
        }
    }
}
static void UserMain22WinFun(void *param)//password
{
    if(FirstFlag[22] == 0)
    {
        FirstFlag[22] = 1;
        NowButtonId=0;
        NowWinid = 22;
        //GuiUpdateDisplayAll();

        tButton[0].x = 0;
        tButton[0].y = 11;
        tButton[0].wide = 128;
        tButton[0].high = 15;
        tButton[0].name = "Current Value";
        tButton[0].linesize = 0;
        tButton[0].flag = 1;/* 按下状态 */
        GuiButton(&tButton[0]);

        tButton[1].x = 0;
        tButton[1].y = 24;
        tButton[1].wide = 128;
        tButton[1].high = 15;
        tButton[1].name = "Calibration Value";
        tButton[1].linesize = 0;
        tButton[1].flag = 0;/* 按下状态 */
        GuiButton(&tButton[1]);

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[22]=0;
        }
        if(K1_Status==RT_EOK)
        {
            tButton[NowButtonId].flag=0;
            GuiButton(&tButton[NowButtonId]);
            NowButtonId++;
            if(NowButtonId==2){NowButtonId=0;}
            tButton[NowButtonId].flag=1;
            GuiButton(&tButton[NowButtonId]);
        }
        if(K2_Status==RT_EOK)
        {
            FirstFlag[22]=0;
            GuiClearScreen(0);
            if(NowButtonId==0)
            {
                GuiWinAdd(&userMain23Win);
            }
            else if(NowButtonId==1)
            {
                GuiWinAdd(&userMain24Win);
            }
            GuiWinDisplay();
        }
    }
}
double TdsValueOffset=0;
uint16_t TdsValueZeroOffset=0;
uint16_t TdsValueOffsetTemp;
static void UserMain23WinFun(void *param)//password
{
    uint8_t CurrentTdsString[10]={0};
    uint32_t TdsValue,result=0;
    if(FirstFlag[23] == 0)
    {
        FirstFlag[23] = 1;
        NowWinid = 23;

        TdsValue = Tds_Work();
        result = (TdsValue - TdsValueZeroOffset)*TdsValueOffset;
        sprintf(CurrentTdsString,"%05d",result);
        GuiRowText(30,25,110,0,CurrentTdsString);
        GuiRowText(70,25,110,0,"us/cm");

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = SingleBack;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        TdsValue = Tds_Work();
        result = (TdsValue - TdsValueZeroOffset)*TdsValueOffset;
        sprintf(CurrentTdsString,"%05d",result);
        GuiRowText(30,25,110,0,"     ");
        GuiRowText(30,25,110,0,CurrentTdsString);
        rt_thread_mdelay(300);
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[23]=0;
        }
        if(K1_Status==RT_EOK)
        {

        }
        if(K2_Status==RT_EOK)
        {

        }
    }
}

static void UserMain24WinFun(void *param)//password
{
    uint8_t CurrentTdsString[10]={0};
    if(FirstFlag[24] == 0)
    {
        FirstFlag[24] = 1;
        NowWinid = 24;
        TdsValueOffsetTemp=0;

        sprintf(CurrentTdsString,"%05d",0);
        GuiRowText(30,25,110,0,CurrentTdsString);
        GuiRowText(70,25,110,0,"us/cm");

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = Back;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);
    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[24]=0;
        }
        if(K1_Status==RT_EOK)
        {
            TdsValueOffsetTemp+=100;
            sprintf(CurrentTdsString,"%05d",TdsValueOffsetTemp);
            GuiRowText(30,25,110,0,CurrentTdsString);
        }
        if(K2_Status==RT_EOK)
        {
            if(TdsValueOffsetTemp==0)
            {
                TdsValueZeroOffset=Tds_Work();
                rt_kprintf("Zero Offset is %d\r\n",TdsValueZeroOffset);
            }
            else

            {
                TdsValueOffset = (double)TdsValueOffsetTemp / (double)(Tds_Work()-TdsValueZeroOffset);
                printf("Offset is %5.3f\r\n",TdsValueOffset);
            }
            GuiClearScreen(0);
            GuiWinDeleteTop();
            GuiWinDisplay();
            FirstFlag[24]=0;
        }
    }
}
static void UserMain25WinFun(void *param)
{
    if(FirstFlag[25] == 0)
    {
        FirstFlag[25] = 1;
        NowWinid = 25;
        GuiClearScreen(0);

        GuiRowText(16,15,110,0,"Backwash Filter");
        GuiRowText(53,30,57,0,"Now");

        tButton[3].x = 0;
        tButton[3].y = 50;
        tButton[3].wide = 128;
        tButton[3].high = 15;
        tButton[3].name = YesOrNo;
        tButton[3].linesize = 0;
        tButton[3].flag = 1;/* 按下状态 */
        GuiButton(&tButton[3]);

    }
    else
    {
        K0_Status = rt_sem_take(K0_Sem, 0);
        K1_Status = rt_sem_take(K1_Sem, 0);
        K2_Status = rt_sem_take(K2_Sem, 0);
        K2_Long_Status = rt_sem_take(K2_Long_Sem, 0);
        if(K2_Long_Status==RT_EOK)
        {

        }
        if(K0_Status==RT_EOK)
        {
            FirstFlag[25]=0;
            GuiClearScreen(0);
            GuiWinInit();
            GuiWinAdd(&userMain1Win);
            GuiWinDisplay();
        }
        if(K1_Status==RT_EOK)
        {


        }
        if(K2_Status==RT_EOK)
        {
            JumptoAutomatic();
        }
    }
}
