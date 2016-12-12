/*===========================================================================
* ��ַ ��http://www.cdebyte.com/   http://yhmcu.taobao.com/                 *
* ���� ������  ԭ �ں͵��ӹ�����  �� �ڰ��ص��ӿƼ����޹�˾                 * 
* �ʼ� ��yihe_liyong@126.com                                                *
* �绰 ��18615799380                                                        *
============================================================================*/

#include "bsp.h"

/*===========================================================================
* ���� ��SClK_Initial() => ��ʼ��ϵͳʱ�ӣ�ϵͳʱ�� = 4MHZ                  *
============================================================================*/
void SClK_Initial(void)
{
    CLK_MasterPrescalerConfig(CLK_MasterPrescaler_HSIDiv4); // 4M
}

/*===========================================================================
* ���� ��GPIO_Initial() => ��ʼ��ͨ��IO�˿�                                 *
============================================================================*/
void GPIO_Initial(void)
{
    // ����LED���� LED(PA6)  
    GPIO_Init(PORT_LED, PIN_LED, GPIO_Mode_Out_PP_High_Slow);
    GPIO_ResetBits(PORT_LED, PIN_LED);    // ����LED
     
    // ����CC1101��ؿ������� CSN(PB4), IRQ(PA2), GDO2(PA3)
    GPIO_Init(PORT_CC_IRQ, PIN_CC_IRQ, GPIO_Mode_In_PU_No_IT);
    GPIO_Init(PORT_CC_GDO2, PIN_CC_GDO2, GPIO_Mode_In_PU_No_IT);
    
    GPIO_Init(PORT_CC_CSN, PIN_CC_CSN, GPIO_Mode_Out_PP_High_Fast);
    GPIO_SetBits(PORT_CC_CSN, PIN_CC_CSN);
}

/*===========================================================================
* ���� USART1_Initial() => ��ʼ������                                 *
============================================================================*/
void USART1_Initial(void)
{
    // ���ڳ�ʼ��
    CLK_PeripheralClockConfig (CLK_Peripheral_USART,ENABLE); //ʹ������ʱ�ӣ�STM8L����ʱ��Ĭ�Ϲر�
    USART_Init(115200,USART_WordLength_8D,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//USART��ʼ����9600��8N1
    
    // USART_ITConfig (USART_IT_RXNE,ENABLE);//ʹ�ܽ����ж�
    USART_Cmd (ENABLE);//ʹ��USART 
}

/*===========================================================================
* ���� ��SPI_Initial() => ��ʼ��SPI                                         *
============================================================================*/
void SPI_Initial(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_SPI, ENABLE);
    
    SPI_DeInit();
    
    // ����SPI��ز���,2��Ƶ��8MHZ��
    SPI_Init(SPI_FirstBit_MSB, SPI_BaudRatePrescaler_2,
         SPI_Mode_Master, SPI_CPOL_Low, SPI_CPHA_1Edge,
         SPI_Direction_2Lines_FullDuplex, SPI_NSS_Soft);

    SPI_Cmd(ENABLE);
    
    // SPI���IO������
    GPIO_Init(PORT_SPI, PIN_MISO, GPIO_Mode_In_PU_No_IT);       // MISO (PB7)
    GPIO_Init(PORT_SPI, PIN_SCLK, GPIO_Mode_Out_PP_High_Fast);  // SCLK (PB5)
    GPIO_Init(PORT_SPI, PIN_MOSI, GPIO_Mode_Out_PP_High_Fast);  // MOSI (PB6)
}

/*===========================================================================
* ���� ��TIM3_Initial() => ��ʼ����ʱ��3����ʱʱ��Ϊ1ms                     *
============================================================================*/
void TIM3_Initial(void)
{
    TIM3_DeInit();

    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);

    // ����Timer3��ز�����ʱ��Ϊ16/16 = 1MHZ����ʱʱ�� = 1000/1000000 = 1ms
    TIM3_TimeBaseInit(TIM3_Prescaler_4, TIM3_CounterMode_Up, 1000);
    TIM3_Set(0);                      // �ر�TIM3
}

/*===========================================================================
* ���� ��SPI_ExchangeByte() => ͨ��SPI�������ݽ���                          * 
* ���� ����Ҫд��SPI��ֵ                                                    * 
* ��� ��ͨ��SPI������ֵ                                                    * 
============================================================================*/
INT8U SPI_ExchangeByte(INT8U input)
{
    SPI_SendData(input);
    while (RESET == SPI_GetFlagStatus(SPI_FLAG_TXE));   // �ȴ����ݴ������	
    while (RESET == SPI_GetFlagStatus(SPI_FLAG_RXNE));  // �ȴ����ݽ������
    return (SPI_ReceiveData());
}

/*===========================================================================
-----------------------------------�ļ�����----------------------------------
===========================================================================*/
