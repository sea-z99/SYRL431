#ifndef __FLASHWORK_H__
#define __FLASHWORK_H__
#endif
#include "stdint.h"

void boot_time(void);
int Flash_Init(void);
uint32_t Flash_Get(uint8_t id);
void Flash_Set(uint8_t id,uint32_t value);
void Flash_Factory(void);
