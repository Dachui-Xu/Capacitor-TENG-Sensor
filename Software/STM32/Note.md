## ADC采样率

ADC时钟：12MHz

低频信号，确保精度，Sampling Time设置最大，即239.5 Cycles

采样时间：(12.5+239.5)/12 = 21 us

每个ADC转换通道数目为5，所以所需时间：21*5 = 105 us

## 定时器

定时器溢出时间计算：

> T=((period+1)∗(psc+1))/(TIM_CLK_Mhz/TIM_ClockDivision)

Prescaler(PSC)设置为7199。

调节period就好，例如period设置为**4999**：

> T=(7200∗5000)/(72000000∗1)=0.5s

## 串口数据

黄LED：电容检测

蓝LED：ADC检测

```shell
CAP,9089,8800,9248,9903
ADC,1589,1610,1600,1577,1606,1621,1605,1609,1619,1618
```



