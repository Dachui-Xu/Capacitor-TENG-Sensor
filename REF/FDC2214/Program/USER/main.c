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
		delay_init();//延时函数初始化	
    uart_init(921600);//串口初始化   
  //  TIM1_36Mhz_Init();//stm32定时器1输出一个36mhz方波，作为fdc2214的时钟。如果板载了40mhz的晶振，这个就不需要了。
    
    IIC_Init();
    FDC2214_Init();
	while(1)
	{
        FDC2214_GetData();
        printf("\r\nCH0~3(%d , %d , %d , %d)",ch0,ch1,ch2,ch3);
		delay_ms(500);	 //延时300ms
	}
 }


 
 
void FDC2214_GetData(void)
{
    FDC2214_GetChannelData(FDC2214_Channel_0, &ch0);  
    FDC2214_GetChannelData(FDC2214_Channel_1, &ch1);
    FDC2214_GetChannelData(FDC2214_Channel_2, &ch2);
    FDC2214_GetChannelData(FDC2214_Channel_3, &ch3);
}

