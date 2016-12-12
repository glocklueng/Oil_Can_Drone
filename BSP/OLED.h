#ifndef _OLED_H_
#define _OLED_H_

#include "STM8l10x_conf.h"
#include "mytypedef.h"

// OLED��ؿ������Ŷ��壬CMD(PD0), CSN(PB3), RST(PB2)
#define PORT_OLED_CMD   GPIOD
#define PIN_OLED_CMD    GPIO_Pin_0

#define PORT_OLED_CSN   GPIOB
#define PIN_OLED_CSN    GPIO_Pin_3

#define PORT_OLED_RST   GPIOB
#define PIN_OLED_RST    GPIO_Pin_2

// OLED��������
#define OLED_CSN_H()    GPIO_SetBits(PORT_OLED_CSN, PIN_OLED_CSN)
#define OLED_CSN_L()    GPIO_ResetBits(PORT_OLED_CSN, PIN_OLED_CSN)

#define OLED_CMD_H()    GPIO_SetBits(PORT_OLED_CMD, PIN_OLED_CMD)
#define OLED_CMD_L()    GPIO_ResetBits(PORT_OLED_CMD, PIN_OLED_CMD)

#define OLED_RST_H()    GPIO_SetBits(PORT_OLED_RST, PIN_OLED_RST)
#define OLED_RST_L()    GPIO_ResetBits(PORT_OLED_RST, PIN_OLED_RST)

// ��غ�������
void LCD_Init(void);            // ��ʼ��OLED
void LCD_Dis_Logo(void);        // ��ʾ��˾logo
void LCD_Dis_Logo1(void);       // ��ʾ��˾logo
void LCD_WrCmd(INT8U cmd);      // д���OLED
void LCD_WrDat(INT8U dt);       // д���ݵ�OLED
void LCD_Fill(INT8U bmp_dat);   // ���OLED

void LCD_Set_Pos(INT8U page, INT8U column);             // ��λ��굽ָ��λ��
void LCD_Dis_Char(INT8U page, INT8U column, char ch);   // ��ʾһ���ַ�
void LCD_Dis_Str(INT8U page, INT8U column, char *str);  // ��ʾһ���ַ�
void LCD_Dis_16x16(INT8U page, INT8U column, INT8U *p_data); // ��ʾһ������

#endif // _OLED_H_

/*===========================================================================
-----------------------------------�ļ�����----------------------------------
===========================================================================*/