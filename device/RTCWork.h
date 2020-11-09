#ifndef __RTCWORK_H__
#define __RTCWORK_H__
#endif
void RTC_Init(void);
void RTC_Clear(void);
uint8_t Detect_RTC_Wakeup(void);
void RTC_Timer_Entry(void);
void RTC_Check(void);
void LowPowerTimerStart(void);
