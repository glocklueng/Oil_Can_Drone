#include "stdio.h" 
#include "bsp.h" 
#include "CC1101.H"

#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PINS  GPIO_Pin_6

volatile u16  Cnt1ms = 0;    // 1ms计数变量，每1ms加一 
int  RecvWaitTime = 0;        // 接收等待时间                
u16  SendCnt = 0;             // 计数发送的数据包数                

                              // 帧头  源地址  目标地址        有效数据                                帧尾
u8 SendBuffer[SEND_LENGTH] = {0x55,   0,    0xff,    '2', '9', '6', '8', '5', '1', '2', '9', '2', 0x0d, 0x0a}; // 从机待发送数据
                              // 帧头  源地址  目标地址    帧尾
u8 AckBuffer[ACK_LENGTH]   = {0x55,  0xff,     0,     0x0d, 0x0a};                                             // 主机应答数据

void TIM3_Set(u8 sta);                         // 设置TIM3的开关   sta:0，关闭   1，开启
void USART1_SendStr(unsigned char *Str);      // USART发送字符串函数                         
void System_Initial(void);                     // 系统初始化
void Sleep_Initial(void);                      // AWU定时唤醒初始化 
u8   RF_SendPacket(u8 *Sendbuffer, u8 length);  // 从机发送数据包
void DelayMs(u16 x); 

// printf支持
int putchar(int c)  
{  
  while(!USART_GetFlagStatus (USART_FLAG_TXE));//等待发送完毕
  USART_SendData8((uint8_t)c);
  return (c);  
}

void Delay(__IO uint16_t nCount)
{
    /* Decrement nCount value */
    while (nCount != 0)
    {
        nCount--;
    }
}

void main(void)
{
    u8 res = 0;
    volatile u8 Timer_30s = 0;
    
    System_Initial();                                 // 初始化系统所有外设               
    
    CC1101Init();                                     // 初始化L01寄存器  
    printf("STM8_CC1101\r\n");                        // 发送字符串，末尾换行
    //printf("%d:我的PI %s = %.1f\r\n", i,Str, PI);   // 不支持浮点数
    
    SendBuffer[1] = TX_Address;                       // 数据包源地址（从机地址）
    printf("Mode:TX\r\n");
    CC1101SetTRMode(TX_MODE);                         // 发送模式 
    
    Sleep_Initial();                                  // AWU定时唤醒初始化 
    printf("1\r\n");
    while(1)
    {
        if(Timer_30s++ == 4)             // 约 2 Min     30* 4
        {
            printf("3\r\n");
            res = RF_SendPacket(SendBuffer, SEND_LENGTH);
            if(res != 0) printf("\r\nSend ERROR:%d\r\n", (int)res);  // 发送失败
            else         printf("\r\nSend  OVER!\r\n");              // 发送成功
            LED_TOG();                      // LED闪烁，用于指示发送成功
            Timer_30s = 0;
        }
        printf("2\r\n");
        halt();//挂起，最低功耗
    }
}

// 设置TIM3的开关
// sta:0，关闭   1，开启
void TIM3_Set(u8 sta)
{
    if(sta)
    {  
        TIM3_SetCounter(0);     // 计数器清空
        TIM3_ITConfig(TIM3_IT_Update,ENABLE);   // 使能TIM3更新中断
        TIM3_Cmd(ENABLE);      // 使能TIM3	
    }
    else 
    {
        TIM3_Cmd(DISABLE);     // 关闭TIM3		   
        TIM3_ITConfig(TIM3_IT_Update,DISABLE);  // 关闭TIM3更新中断
    }
}

/*===========================================================================
* 函数 : DelayMs() => 延时函数(ms级)                                        *
* 输入 ：x, 需要延时多少(0-65535)                                             *
============================================================================*/
void DelayMs(u16 x)
{
    Cnt1ms = 0;
    TIM3_Set(1);
    while(Cnt1ms <= x);
    TIM3_Set(0);
}

/*===========================================================================
* 函数 ：TIM3_1MS_ISR() => 定时器3服务函数, 定时时间基准为1ms               *
============================================================================*/
void TIM3_1MS_ISR(void)
{
    Cnt1ms++;
    
    if(RecvWaitTime > 0) RecvWaitTime--;    // 数据接收计时
}

/*===========================================================================
* 函数: System_Initial() => 初始化系统所有外设                              *
============================================================================*/
void System_Initial(void)
{
    SClK_Initial();         // 初始化系统时钟，16M     
    GPIO_Initial();         // 初始化GPIO   LED KEY 
 
    USART1_Initial();       // 初始化串口1  
    TIM3_Initial();         // 初始化定时器3，基准1ms  
    SPI_Initial();          // 初始化SPI               
    //enableInterrupts();     // 打开总中断            
}

/*===========================================================================
* 函数 : BSP_RF_SendPacket() => 无线发送数据函数                            *
* 输入 ：Sendbuffer指向待发送的数据，length发送数据长度                      *
* 输出 ：0，发送成功                                                      
         1，等待应答超时
         2，数据包长度错误
         3，数据包帧头错误
         4，数据包源地址错误
         5，数据包目标地址错误
         6，数据包帧尾
============================================================================*/
INT8U RF_SendPacket(INT8U *Sendbuffer, INT8U length)
{
    uint8_t  i = 0, ack_len = 0, ack_buffer[65] = {0};
    
    CC1101SendPacket(SendBuffer, length, ADDRESS_CHECK);    // 发送数据 
                           
    CC1101Init();                               // 初始化L01寄存器   
    CC1101SetTRMode(RX_MODE);                   // 准备接收应答

    RecvWaitTime = (int)RECV_TIMEOUT;           // 等待应答超时限制1500ms
    TIM3_Set(1);                                // 开启TIM3

    printf("Send Over, waiting for ack...\r\n");
    while(CC_IRQ_READ() != 0)                   // 等待接收数据包
    {
        if(RecvWaitTime <= 0)      
        {  
            TIM3_Set(0);                            // 关闭TIM3
            printf("RecvWaitTime0=%d\r\n", RecvWaitTime);
            return 1;                              // 等待应答超时
        }
    }
    printf("RecvWaitTime1=%d\r\n", RecvWaitTime);
    while(CC_IRQ_READ() == 0);               
    printf("RecvWaitTime2=%d\r\n", RecvWaitTime);
    TIM3_Set(0);                                // 关闭TIM3
    ack_len = CC1101RecPacket(ack_buffer);      // 读取收到的数据
    
//                        // 帧头  源地址  目标地址    帧尾
//AckBuffer[ACK_LENGTH]   = {0x55,  0xff,     0,     0x0d, 0x0a};                                             // 主机应答数据
    
    if(ack_len != ACK_LENGTH) return 2;                                               // 数据包长度错误
    if(ack_buffer[0] != 0x55) return 3;                                               // 数据包帧头错误
    if(ack_buffer[1] != 0xff) return 4;                                               // 数据包源地址错误       
    if(ack_buffer[2] == 0xff) return 5;                                               // 数据包目标地址错误
    if((ack_buffer[ack_len-2] != 0x0d) || (ack_buffer[ack_len-1] != 0x0a)) return 6;  // 数据包帧尾

    // 应答正确
    printf("ack_len=%d;ack_buffer:", (int)ack_len);
    for(i = 0; i < ack_len; i++)                     
    {
        printf("%d ", (int)ack_buffer[i]);
    }
    printf("\r\n");

    return 0;  
}

/*******************************************************************************
****入口参数：需要发送的字符串
****出口参数：无
****函数备注：USART发送字符串函数
*******************************************************************************/
void USART1_SendStr(unsigned char *Str) 
{
    while(*Str!=0)//不为结束
    {
        USART_SendData8(*Str);     //发送数据 
        while(!USART_GetFlagStatus (USART_FLAG_TXE));//等待发送完毕
        Str++;//下一个数据
    }
}

// AWU定时唤醒初始化
void Sleep_Initial(void)
{
    // 所有IO全部输出低电平    降低功耗
    GPIO_Init(GPIOA, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOB, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOC, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOD, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
    
    CLK_PeripheralClockConfig(CLK_Peripheral_AWU, ENABLE); // 使能AWU外设时钟
    
    AWU_DeInit();                                          // AWU恢复初始状态
    AWU_LSICalibrationConfig(12800);                       // AWU LSI校准，因为AWU是被LSI驱动工作的
    AWU_Init(AWU_Timebase_2s);                            // 30s定时唤醒
    AWU_Cmd(ENABLE);                                       // 使能AWU
    enableInterrupts();                                    // 使能中断
}