#ifndef __LCD_DISPLAY_H
#define	__LCD_DISPLAY_H
#endif

struct rt_event lcd_jump_event;
#define JUMP_TDS (1 << 1)
#define JUMP_STALLING (1 << 2)
#define JUMP_FINISH (1 << 3)
#define JUMP_NOMOTO (1 << 4)
#define JUMP_EXIT (1 << 5)

void lcd(void);
void JumptoReminder(void);
void JumptoAutomatic(void);
void JumpToBatteryEmpty(void);
void JumpToBatteryNew(void);
void jumpa(void);
void jumpb(void);
void jumpc(void);
void jumpd(void);
void jumpe(void);
void JumptoBackwash(void);
void userAppPortInit(void);
void LcdtoBackwash(void);

