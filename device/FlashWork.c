#include "rtthread.h"
#include "rtdevice.h"
#include "pin_config.h"
#include "FlashWork.h"
#include <easyflash.h>
#include <fal.h>
#include "spi_flash_sfud.h"
#include "drv_spi.h"
#include "pin_config.h"
rt_spi_flash_device_t w25q16fw;

#define DBG_TAG "Flash"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

typedef struct _env_list {
    char *key;
} env_list;
const env_list Key_list[20] = {
    {"boot_times"},							//0
    {"Reminder_Week"},					    //1
    {"Reminder_Day"},						//2
    {"Reminder_Enable"},				    //3
    {"Automatic_Week"},					    //4
    {"Automatic_Day"},					    //5
    {"Automatic_Enable"},				    //6
    {"Deltapress_Enable"},			        //7
    {"Counter_Manual"},				    	//8
    {"Counter_Automatic"},			        //9
    {"Counter_Deltapress"},			        //10
    {"Counter_Error"},					    //11
    {"Setting_Deltapress"},			        //12
    {"Setting_Hardness"},			    	//13
    {"Setting_Backwashtime"},		        //14
    {"Setting_Language"},			    	//15
    {"RTC_Reminder_Time"},			        //16
    {"RTC_Automatic_Time"},			        //17
    {"TDS_Offset"}							//18
};
void boot_time(void)
{
    uint32_t i_boot_times = RT_NULL;
    char *c_old_boot_times, c_new_boot_times[11] = {0};

    /* 从环境变量中获取启动次数 */
    c_old_boot_times = ef_get_env("boot_times");
    /* 获取启动次数是否失败 */
    if (c_old_boot_times == RT_NULL)
        c_old_boot_times[0] = '0';

    i_boot_times = atol(c_old_boot_times);
    /* 启动次数加 1 */
    i_boot_times++;
    LOG_D("The system now boot %d times", i_boot_times);
    /* 数字转字符串 */
    sprintf(c_new_boot_times, "%ld", i_boot_times);
    /* 保存开机次数的值 */
    ef_set_env("boot_times", c_new_boot_times);
    ef_save_env();
}
int flash_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    rt_pin_mode(FLASH_EN, PIN_MODE_OUTPUT);
    rt_pin_write(FLASH_EN,1);
    rt_hw_spi_device_attach("spi2", "spi20", GPIOB, GPIO_PIN_12);
    w25q16fw = rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi20");
    if (RT_NULL == w25q16fw)
    {
        return -RT_ERROR;
    };
    return RT_EOK;
}
uint32_t Flash_Get(uint8_t id)
{
    uint32_t Counter = 0;
    char *Counter_Temp;
    Counter_Temp = ef_get_env((const char*)&Key_list[id]);
    if (Counter_Temp == RT_NULL)
		Counter_Temp[0] = '0';
    Counter = atol(Counter_Temp);
    LOG_D("Reading From %s is %d\r\n", Key_list[id],Counter);
    rt_thread_mdelay(10);
    return Counter;
}
MSH_CMD_EXPORT(Flash_Get,Counter_Get);
void Flash_Set(uint8_t id,uint32_t value)
{
    char * Counter_Temp;
    Counter_Temp = rt_malloc(32);
    snprintf(Counter_Temp, 32,"%ld", value);
    ef_set_env((const char*)&Key_list[id], Counter_Temp);
    rt_free(Counter_Temp);
    LOG_D("Writing to %s is %d\r\n", Key_list[id],value);
    rt_thread_mdelay(10);
}
void Flash_Factory(void)
{
    for(uint8_t i=0;i<=17;i++)
    {
        Flash_Set(i,0);
    }
}
MSH_CMD_EXPORT(Flash_Factory,Flash_Factory);
