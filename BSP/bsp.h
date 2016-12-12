/*===========================================================================
��ַ ��http://yhmcu.taobao.com/
���� ������  ԭ �ں͵��ӹ�����  �� �ڰ��ص��ӿƼ����޹�˾ 
�ʼ� ��yihe_liyong@126.com
�绰 ��18615799380
===========================================================================*/

#ifndef _BSP_H_
#define _BSP_H_

#include "STM8l10x_conf.h"
#include "CC1101.h"
#include "mytypedef.h"
#include "OLED.h"

// SPI���Ŷ��� SCLK(PB5), MOSI(PB6), MISO(PB7)
#define PORT_SPI        GPIOB
#define PIN_SCLK        GPIO_Pin_5        
#define PIN_MOSI        GPIO_Pin_6
#define PIN_MISO        GPIO_Pin_7

// LED��KEY���Ŷ��壬LED(PA6), KEY(PA4)
#define PORT_LED        GPIOA
#define PIN_LED         GPIO_Pin_6

#define PORT_KEY        GPIOB
#define PIN_KEY         GPIO_Pin_4

// LED����������(ON)��, (OFF)�رգ�(TOG)��ת
#define LED_ON()        GPIO_ResetBits(PORT_LED, PIN_LED)        
#define LED_OFF()       GPIO_SetBits(PORT_LED, PIN_LED)
#define LED_TOG()       GPIO_ToggleBits(PORT_LED, PIN_LED)

void SClK_Initial(void);                // ��ʼ��ϵͳʱ�ӣ�ϵͳʱ�� = 16MHZ
void GPIO_Initial(void);                // ��ʼ��ͨ��IO�˿� LED KEY
void USART1_Initial(void);              // ��ʼ��USART1
void SPI_Initial(void);                 // ��ʼ��SPI
void TIM3_Initial(void);                // ��ʼ����ʱ��3����ʱʱ��Ϊ1ms
INT8U SPI_ExchangeByte(INT8U input);     // ͨ��SPI�������ݽ��� 

#endif //_BSP_H_

/*===========================================================================
-----------------------------------�ļ�����----------------------------------
===========================================================================*/
