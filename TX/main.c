#include "stdio.h" 
#include "bsp.h" 
#include "CC1101.H"

volatile u16  Cnt1ms = 0;    // 1ms����������ÿ1ms��һ 
int  RecvWaitTime = 0;        // ���յȴ�ʱ��                
u16  SendCnt = 0;             // �������͵����ݰ���                

                              // ֡ͷ  Դ��ַ  Ŀ���ַ        ��Ч����                                ֡β
u8 SendBuffer[SEND_LENGTH] = {0x55,   0,    0xff,    '2', '9', '6', '8', '5', '1', '2', '9', '2', 0x0d, 0x0a}; // �ӻ�����������
                              // ֡ͷ  Դ��ַ  Ŀ���ַ    ֡β
u8 AckBuffer[ACK_LENGTH]   = {0x55,  0xff,     0,     0x0d, 0x0a};                                             // ����Ӧ������

void TIM3_Set(u8 sta);                         // ����TIM3�Ŀ���   sta:0���ر�   1������
void USART1_SendStr(unsigned char *Str);      // USART�����ַ�������                         
void System_Initial(void);                     // ϵͳ��ʼ��
u8   RF_SendPacket(u8 *Sendbuffer, u8 length);  // �ӻ��������ݰ�
void DelayMs(u16 x); 

// printf֧��
int putchar(int c)  
{  
  while(!USART_GetFlagStatus (USART_FLAG_TXE));//�ȴ��������
  USART_SendData8((uint8_t)c);
  return (c);  
}

void main(void)
{
    u8 res = 0;
    
    System_Initial();                                 // ��ʼ��ϵͳ��������               
    
    CC1101Init();                                     // ��ʼ��L01�Ĵ���  
    printf("STM8_CC1101\r\n");                        // �����ַ�����ĩβ����
    //printf("%d:�ҵ�PI %s = %.1f\r\n", i,Str, PI);   // ��֧�ָ�����
    
    SendBuffer[1] = TX_Address;  // ���ݰ�Դ��ַ���ӻ���ַ��
    printf("Mode:TX\r\n");
    CC1101SetTRMode(TX_MODE);    // ����ģʽ 
    while(1)
    {
        res = RF_SendPacket(SendBuffer, SEND_LENGTH);
        if(res != 0) printf("\r\nSend ERROR:%d\r\n", (int)res);  // ����?���䨪?���䨲??
        LED_TOG();                      // LED��˸������ָʾ���ͳɹ�
        DelayMs(1000);
        DelayMs(1000);
    }
}

// ����TIM3�Ŀ���
// sta:0���ر�   1������
void TIM3_Set(u8 sta)
{
    if(sta)
    {  
        TIM3_SetCounter(0);     // ���������
        TIM3_ITConfig(TIM3_IT_Update,ENABLE);   // ʹ��TIM3�����ж�
        TIM3_Cmd(ENABLE);      // ʹ��TIM3	
    }
    else 
    {
        TIM3_Cmd(DISABLE);     // �ر�TIM3		   
        TIM3_ITConfig(TIM3_IT_Update,DISABLE);  // �ر�TIM3�����ж�
    }
}

/*===========================================================================
* ���� : DelayMs() => ��ʱ����(ms��)                                        *
* ���� ��x, ��Ҫ��ʱ����(0-65535)                                             *
============================================================================*/
void DelayMs(u16 x)
{
    Cnt1ms = 0;
    TIM3_Set(1);
    while(Cnt1ms <= x);
    TIM3_Set(0);
}

/*===========================================================================
* ���� ��TIM3_1MS_ISR() => ��ʱ��3������, ��ʱʱ���׼Ϊ1ms               *
============================================================================*/
void TIM3_1MS_ISR(void)
{
    Cnt1ms++;
    
    if(RecvWaitTime > 0) RecvWaitTime--;    // ���ݽ��ռ�ʱ
}

/*===========================================================================
* ����: System_Initial() => ��ʼ��ϵͳ��������                              *
============================================================================*/
void System_Initial(void)
{
    SClK_Initial();         // ��ʼ��ϵͳʱ�ӣ�16M     
    GPIO_Initial();         // ��ʼ��GPIO   LED KEY 
 
    USART1_Initial();       // ��ʼ������1  
    TIM3_Initial();         // ��ʼ����ʱ��3����׼1ms  
    SPI_Initial();          // ��ʼ��SPI               

    enableInterrupts();     // �����ж�            
}

/*===========================================================================
* ���� : BSP_RF_SendPacket() => ���߷������ݺ���                            *
* ���� ��Sendbufferָ������͵����ݣ�length�������ݳ���                      *
* ��� ��0�����ͳɹ�                                                      
         1���ȴ�Ӧ��ʱ
         2�����ݰ����ȴ���
         3�����ݰ�֡ͷ����
         4�����ݰ�Դ��ַ����
         5�����ݰ�Ŀ���ַ����
         6�����ݰ�֡β
============================================================================*/
INT8U RF_SendPacket(INT8U *Sendbuffer, INT8U length)
{
    uint8_t  i = 0, ack_len = 0, ack_buffer[65] = {0};
    
    CC1101SendPacket(SendBuffer, length, ADDRESS_CHECK);    // �������� 
                           
    CC1101Init();                               // ��ʼ��L01�Ĵ���   
    CC1101SetTRMode(RX_MODE);                   // ׼������Ӧ��

    RecvWaitTime = (int)RECV_TIMEOUT;           // �ȴ�Ӧ��ʱ����1500ms
    TIM3_Set(1);                                // ����TIM3

    printf("Send Over, waiting for ack...\r\n");
    while(CC_IRQ_READ() != 0)                   // �ȴ��������ݰ�
    {
        if(RecvWaitTime <= 0)      
        {  
            TIM3_Set(0);                            // �ر�TIM3
            printf("RecvWaitTime0=%d\r\n", RecvWaitTime);
            return 1;                              // �ȴ�Ӧ��ʱ
        }
    }
    printf("RecvWaitTime1=%d\r\n", RecvWaitTime);
    while(CC_IRQ_READ() == 0);               
    printf("RecvWaitTime2=%d\r\n", RecvWaitTime);
    TIM3_Set(0);                                // �ر�TIM3
    ack_len = CC1101RecPacket(ack_buffer);      // ��ȡ�յ�������
    
//                        // ֡ͷ  Դ��ַ  Ŀ���ַ    ֡β
//AckBuffer[ACK_LENGTH]   = {0x55,  0xff,     0,     0x0d, 0x0a};                                             // ����Ӧ������
    
    if(ack_len != ACK_LENGTH) return 2;                                               // ���ݰ����ȴ���
    if(ack_buffer[0] != 0x55) return 3;                                               // ���ݰ�֡ͷ����
    if(ack_buffer[1] != 0xff) return 4;                                               // ���ݰ�Դ��ַ����       
    if(ack_buffer[2] == 0xff) return 5;                                               // ���ݰ�Ŀ���ַ����
    if((ack_buffer[ack_len-2] != 0x0d) || (ack_buffer[ack_len-1] != 0x0a)) return 6;  // ���ݰ�֡β

    // Ӧ����ȷ
    printf("ack_len=%d;ack_buffer:", (int)ack_len);
    for(i = 0; i < ack_len; i++)                     
    {
        printf("%d ", (int)ack_buffer[i]);
    }
    printf("\r\n");

    return 0;  
}

/*******************************************************************************
****��ڲ�������Ҫ���͵��ַ���
****���ڲ�������
****������ע��USART�����ַ�������
*******************************************************************************/
void USART1_SendStr(unsigned char *Str) 
{
    while(*Str!=0)//��Ϊ����
    {
        USART_SendData8(*Str);     //�������� 
        while(!USART_GetFlagStatus (USART_FLAG_TXE));//�ȴ��������
        Str++;//��һ������
    }
}