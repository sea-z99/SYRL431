#ifndef __MOTOCONTROL_H__
#define __MOTOCONTROL_H__
#endif

#define Event_Moto_Free             (1 << 0)
#define Event_Moto_Forward          (1 << 1)
#define Event_Moto_Back             (1 << 2)
#define Event_Moto_Cycle            (1 << 3)

void Moto_Init(void);
void Moto_TaskUp(void);
void Moto_Cycle(void);
void Moto_Pin_Init(void);
void Moto_Pin_DeInit(void);
