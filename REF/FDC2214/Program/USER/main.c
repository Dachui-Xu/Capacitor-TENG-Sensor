#include "delay.h"
#include "sys.h"
#include "stdio.h"
#include "usart.h"
#include "timer.h"
#include "FDC2214.h"

void FDC2214_GetData(void);
u32 ch0,ch1,ch2,ch3;

 int main(void)
 {	
		delay_init();//��ʱ������ʼ��	
    uart_init(921600);//���ڳ�ʼ��   
  //  TIM1_36Mhz_Init();//stm32��ʱ��1���һ��36mhz��������Ϊfdc2214��ʱ�ӡ����������40mhz�ľ�������Ͳ���Ҫ�ˡ�
    
    IIC_Init();
    FDC2214_Init();
	while(1)
	{
        FDC2214_GetData();
        printf("\r\nCH0~3(%d , %d , %d , %d)",ch0,ch1,ch2,ch3);
		delay_ms(500);	 //��ʱ300ms
	}
 }


 
 
void FDC2214_GetData(void)
{
    FDC2214_GetChannelData(FDC2214_Channel_0, &ch0);  
    FDC2214_GetChannelData(FDC2214_Channel_1, &ch1);
    FDC2214_GetChannelData(FDC2214_Channel_2, &ch2);
    FDC2214_GetChannelData(FDC2214_Channel_3, &ch3);
}

