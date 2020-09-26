#include "rtthread.h"
#include "rtdevice.h"
#include "stm32l4xx.h"
#include "pin_config.h"
#include "TdsWork.h"
#include "adcwork.h"
#include "string.h"

#define DBG_TAG "TDS"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

uint8_t data_parsing_id = 0;
uint32_t TDS_Value = 0;


#define TDS_UART_NAME                   "uart2"
#define DATA_CMD_BEGIN                   0x55       /* 结束位设置为 \r，即回车符 */
#define ONE_DATA_MAXLEN                  11         /* 不定长数据的最大长度 */

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;
static rt_thread_t TDS_Uart_Thread = RT_NULL;
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */

/* 接收数据回调函数 */
static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    if (size > 0)
    {
        rt_sem_release(&rx_sem);
    }
    return RT_EOK;
}

static char uart_sample_get_char(void)
{
    char ch;

    while (rt_device_read(serial, 0, &ch, 1) == 0)
    {
        rt_sem_control(&rx_sem, RT_IPC_CMD_RESET, RT_NULL);
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

/* 数据解析线程 */
void data_parsing(void)
{
    char ch;
    char data[ONE_DATA_MAXLEN];
    Tds_Init();
    LOG_D("Tds Thread Init Success\r\n");

    while (1)
    {
        ch = uart_sample_get_char();
        data[data_parsing_id++] = ch;
        if(data_parsing_id == ONE_DATA_MAXLEN)
        {
            if(data[0] == DATA_CMD_BEGIN)
            {
                LOG_D("TDS_High=%x,TDS_Low=%x\r\n",data[4],data[5]);
                TDS_Value = ((data[4]<<8) + data[5]);
                TDS_Value = (int)(0.1 * TDS_Value);
                LOG_D("TDS_Value is %d\r\n",TDS_Value);
            }
            else
            {
                LOG_D("TDS_Value Error\r\n");
            }
            data_parsing_id = 0;
        }
    }
}

void TDS__Uart_Init(void)
{
    char uart_name[RT_NAME_MAX];

    rt_strncpy(uart_name, TDS_UART_NAME, RT_NAME_MAX);

    /* 查找系统中的串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial)
    {
        LOG_D("find %s failed!\n", uart_name);
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 串口参数配置 */
    config.baud_rate = BAUD_RATE_9600;        //修改波特率为 9600
    config.data_bits = DATA_BITS_8;           //数据位 8
    config.stop_bits = STOP_BITS_1;           //停止位 1
    config.bufsz     = 128;                   //修改缓冲区 buff size 为 128
    config.parity    = PARITY_NONE;           //无奇偶校验位
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_rx_ind);

    /* 创建 serial 线程 */
    TDS_Uart_Thread = rt_thread_create("serial", (void (*)(void *parameter))data_parsing, RT_NULL, 1024, 25, 10);

    /* 创建成功则启动线程 */
    if (TDS_Uart_Thread != RT_NULL)
    {
        rt_thread_startup(TDS_Uart_Thread);
    }
    LOG_D("Tds Init Success\r\n");
}
MSH_CMD_EXPORT(TDS__Uart_Init, TDS__Uart_Init);
void Tds_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Alternate = 7;
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    rt_pin_mode(TDS_EN, PIN_MODE_OUTPUT);
    rt_pin_write(TDS_EN,1);
}
void Tds_DeInit(void)
{
    rt_pin_mode(TDS_EN, PIN_MODE_OUTPUT);
    rt_pin_write(TDS_EN,0);
    __HAL_RCC_USART2_CLK_DISABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
uint32_t Tds_Work(void)
{
    uint32_t Data=0;
    LOG_D("Read TDS Now\r\n");
    char TdsRead[]={0x55,0x07,0x05,0x01,0x00,0x00,0x00,0x62};
    Data = TDS_Value;
    TDS_Value = 0;
    data_parsing_id = 0;
    rt_device_write(serial, 0, TdsRead, 8);
    return Data;
}
MSH_CMD_EXPORT(Tds_Work,Tds_Work);
