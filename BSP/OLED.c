#include "OLED.h"
#include "font.h"

extern INT8U SPI_ExchangeByte(INT8U input); // ͨ��SPI�������ݽ���,��bsp.c

/*===========================================================================
* ���� ��LCD_Dis_16x16() => ��ʾһ��16X16���ַ������֣�                     * 
* ���� ��page��0-7����column��0-127����ʾ��ʾλ�ã�p_dataָ����Ҫ��ʾ������ * 
============================================================================*/
void LCD_Dis_16x16(INT8U page, INT8U column, INT8U *p_data)
{
    INT8U i;
    
    // ��ʾ��һҳ������
    LCD_Set_Pos(page, column);
    for (i=0; i<16; i++)    { LCD_WrDat(*p_data++); }
    
    // ��ʾ�ڶ�ҳ������
    LCD_Set_Pos(page+1, column);
    for (i=0; i<16; i++)    { LCD_WrDat(*p_data++); }
}

/*===========================================================================
* ���� ��LCD_Dis_Logo() => ��ʾ��˾logo                                     * 
* ˵�� ����ʾ�ڵ�һ�У���ʾ����Ϊ���ں͵��ӡ�                                 * 
============================================================================*/
void LCD_Dis_Logo(void)
{
    INT8U i = 0;
    
    LCD_Dis_16x16(0, i*16+32, (INT8U *)&Our_Logo[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+32, (INT8U *)&Our_Logo[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+32, (INT8U *)&Our_Logo[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+32, (INT8U *)&Our_Logo[32*i]);
}

/*===========================================================================
* ���� ��LCD_Dis_Logo1() => ��ʾ��˾logo                                     * 
* ˵�� ����ʾ�ڵ�һ�У���ʾ����Ϊ���ɶ��ڰ��ء�                               * 
============================================================================*/
void LCD_Dis_Logo1(void)
{
    INT8U i = 0;
    
    LCD_Dis_16x16(0, i*16+16, (INT8U *)&Our_Logo1[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+16, (INT8U *)&Our_Logo1[32*i]);
    i++;
//    LCD_Dis_16x16(0, i*16+16, (INT8U *)&Our_Logo1[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+16, (INT8U *)&Our_Logo1[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+16, (INT8U *)&Our_Logo1[32*i]);
    i++;
    LCD_Dis_16x16(0, i*16+16, (INT8U *)&Our_Logo1[32*i]);
}

/*===========================================================================
* ���� ��LCD_Set_Pos() => ��λ��굽ָ��λ��                                * 
* ���� ��page��0-7����column��0-127����ʾ����Ŀ��λ��                     * 
============================================================================*/
void LCD_Set_Pos(INT8U page, INT8U column)
{
    LCD_WrCmd(0xb0 + (page&0x07));
    LCD_WrCmd(0x10 + ((column>>4)&0x0F));
    LCD_WrCmd(column & 0x0F);
}

/*===========================================================================
* ���� ��LCD_Dis_Char() => ��ʾһ���ַ�                                     * 
* ���� ��page��0-7����column��0-127����ʾ��ʾλ�ã�ch��Ҫ��ʾ���ַ�         * 
============================================================================*/
void LCD_Dis_Char(INT8U page, INT8U column, char ch)
{
    INT8U   j = 0;
    INT16U  pos = 0;
    
    if (ch < ' ')           { return; }

    pos = 16 * (ch-' ');    
    LCD_Set_Pos(page, column);
    for (j=0; j<8; j++)     { LCD_WrDat(Font8x16[pos++]); }    
    LCD_Set_Pos(page+1, column);
    for (j=0; j<8; j++)     { LCD_WrDat(Font8x16[pos++]); }
}

/*===========================================================================
* ���� ��LCD_Dis_Str() => ��ʾһ���ַ�                                      * 
* ���� ��page��0-7����column��0-127����ʾ��ʾλ�ã�strָ����ʾ���ַ���      * 
============================================================================*/
void LCD_Dis_Str(INT8U page, INT8U column, char *str)
{
    while (*str)
    {
        LCD_Dis_Char(page, column, *str++);
        column += 8;
    }
}

/*===========================================================================
* ���� ��LCD_WrCmd() => д���OLED                                        * 
* ���� ��cmd��ʾ��Ҫд�������                                              * 
============================================================================*/
void LCD_WrCmd(INT8U cmd)
{
    OLED_CSN_L();
    OLED_CMD_L();
    SPI_ExchangeByte(cmd);
    OLED_CSN_H();
}

/*===========================================================================
* ���� ��LCD_WrDat() => д���ݵ�OLED                                        * 
* ���� ��dt��ʾ��Ҫд�������                                               * 
============================================================================*/
void LCD_WrDat(INT8U dt)
{
    OLED_CSN_L();
    OLED_CMD_H();
    SPI_ExchangeByte(dt);
    OLED_CSN_H();
}

/*===========================================================================
* ���� ��LCD_Fill() => ���OLED                                             * 
* ���� ��bmp_dat��ʾ��Ҫ��������                                          * 
============================================================================*/
void LCD_Fill(INT8U bmp_dat)
{
    INT8U y, x;
    
    for (y=0; y<8; y++)
    {
        LCD_WrCmd(0xb0 + y);
        LCD_WrCmd(0x01);
        LCD_WrCmd(0x10);
        for (x=0; x<128; x++)
        LCD_WrDat(bmp_dat);
    }
}

/*===========================================================================
* ���� ��LCD_Init() => ��ʼ��OLED                                           * 
============================================================================*/
void LCD_Init(void)
{
    INT16U x = 0;
    
    
    OLED_RST_L();               // ��λOLED
    for (x=0; x<1000; x++);     // ��ʱ���ȴ�OLED��λ���
    OLED_RST_H();
    
    LCD_WrCmd(0xae);    // �ر�OLED���
    LCD_WrCmd(0x00);    // set low column address
    LCD_WrCmd(0x10);    // set high column address
    LCD_WrCmd(0x40);    // Set Mapping RAM Display Start Line (0x00~0x3F)
    LCD_WrCmd(0x81);    // set contrast control register
    LCD_WrCmd(0xcf);    // Set SEG Output Current Brightness
    LCD_WrCmd(0xa1);    // Set SEG/Column Mapping       0xa0���ҷ��� 0xa1����
    LCD_WrCmd(0xc8);    // Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    LCD_WrCmd(0xa6);    // set normal display
    LCD_WrCmd(0xa8);    // set multiplex ratio(1 to 64)
    LCD_WrCmd(0x3f);    // 1/64 duty
    LCD_WrCmd(0xd3);    // set display offset Shift Mapping RAM Counter
    LCD_WrCmd(0x00);    // not offset
    LCD_WrCmd(0xd5);    // set display clock divide ratio/oscillator frequency
    LCD_WrCmd(0x80);    // set divide ratio, Set Clock as 100 Frames/Sec
    LCD_WrCmd(0xd9);    // set pre-charge period
    LCD_WrCmd(0xf1);    // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    LCD_WrCmd(0xda);    // set com pins hardware configuration
    LCD_WrCmd(0x12);
    LCD_WrCmd(0xdb);    // set vcomh
    LCD_WrCmd(0x40);    // Set VCOM Deselect Level
    LCD_WrCmd(0x20);    // Set Page Addressing Mode (0x00/0x01/0x02)
    LCD_WrCmd(0x02);    //
    LCD_WrCmd(0x8d);    // set Charge Pump enable/disable
    LCD_WrCmd(0x14);    // set(0x10) disable
    LCD_WrCmd(0xa4);    // Disable Entire Display On (0xa4/0xa5)
    LCD_WrCmd(0xa6);    // Disable Inverse Display On (0xa6/a7)
    LCD_WrCmd(0xaf);    // turn on oled panel
    LCD_Fill(0x00);     // ��ʼ����
}

/*===========================================================================
-----------------------------------�ļ�����----------------------------------
===========================================================================*/
