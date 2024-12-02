## Stm32串口下载电路

最新推荐文章于 2024-06-18 15:55:30 发布

![](https://csdnimg.cn/release/blogv2/dist/pc/img/original.png)

[DaiWenQina](https://blog.csdn.net/DaiWenQina "DaiWenQina") ![](https://csdnimg.cn/release/blogv2/dist/pc/img/newCurrentTime2.png) 最新推荐文章于 2024-06-18 15:55:30 发布

版权声明：本文为博主原创文章，遵循 [CC 4.0 BY-SA](http://creativecommons.org/licenses/by-sa/4.0/) 版权协议，转载请附上原文出处链接和本声明。

ISP（in\_system\_programing）在线系统编程简介：

1.  ISP下载是指通过串口将程序下载到Flash中，然后自动运行的程序的启动方式。
2.  ISP需要用到Bootloader（自举程序），bootloader存储在stm32内部的自举Rom存储器中，作用是接收串口外设发送的程序数据放置到flash中。

Stm32启动方式：

<table><tbody><tr><td colspan="2" rowspan="1">启动模式选择引脚</td><td colspan="1" rowspan="2">启动模式</td><td colspan="1" rowspan="2">说明</td></tr><tr><td>Boot0</td><td>Boot1</td></tr><tr><td>0</td><td>X</td><td>内部Flash（513K）</td><td>从Flash中加载代码启动</td></tr><tr><td>1</td><td>0</td><td>系统存储器</td><td>通过ISP加载代码</td></tr><tr><td>1</td><td>1</td><td>内置SRAM</td><td>内置SRAM加载代码启动</td></tr></tbody></table>

![](https://i-blog.csdnimg.cn/blog_migrate/99571eb1a504c093ee2cb202ad8c7b68.png)  
ISP普通下载：

1.  电脑上位机通过USB-Type-C线连接到Stm32的USART1。
2.  设置Boot1固定低电平，Boot0为高电平，
3.  按下复位按钮，此时单片机进入bootloader模块准备接受数据。
4.  上位机开始发程序，单片机通过串口接收代码数据存入Flash中
5.  下载完成后，跳帽设置Boot0为低电平，按下Reset按钮，单片机进入Flash启动代码。

![](https://i-blog.csdnimg.cn/blog_migrate/6644cdd203023f0543d8aac4b2802e40.png)  
ISP一键下载：

注：上位机通过控制DTR、RTS来对RESET进行复位和对BOOT0设置，以达到ISP自动下载效果

1.  Boot1默认接地为0
2.  RTS#为0、DTR#为1，Q3、Q5导通，Reset被拉低，BOOT0拉高，单片机进入复位
3.  RTS#保持为0、DTR#为0，则Q3导通，Q5截止，Reset恢复高电平，Boot0依旧拉高，此时串口接受来自上位的代码数据并可选择立即执行代码。
4.  接受完之后，RTS#、DTR#都置1，Q3、Q5均截止，Boot0恢复低电平，下次重新上电或RESET时，单片机执行内部Flash代码。

![](https://i-blog.csdnimg.cn/blog_migrate/1f00c22f91ec94595662e0d353440eb8.png)

备注：时间轴1-2为擦除，2-3时间段为下载代码。用内部一段IO代码作为程序运行标志。



## 串口下载电路

先放原理图（补充：图中的BOOT0通过10K的电阻接到地），再解释为什么这么设计：

![img](https://images2018.cnblogs.com/blog/1184137/201802/1184137-20180227114811312-391202447.png)

STM32启动方式：BOOT0和 BOOT1用于设置 STM32的启动方式 ，见下表：

![img](https://images2018.cnblogs.com/blog/1184137/201802/1184137-20180227135207082-1063105893.png)

BOOT0=1,BOOT1=0:串口下载模式；

BOOT0=0,BOOT1=X:程序启动模式，从内部flash启动。

设计思路：串口一键下载需要做的就是下载时，为串口下载模式，下载完成后，为启动模式。

电路设计：一般在使用软件进行串口烧录程序的时候用到这个电路（这不是废话吗），例如使用FlyMCU软件下载程序；CH340G芯片的13脚DTR、14脚RTS的电平是可以被下载软件控制的。通过串口的DTR 和 RTS 信号，来自动配置 BOOT0 和 RST 信号。他们的状态如下：

　　> RTS#和DTR#初始状态时为高电平；

　　> 在启用下载的时候，DTR#维持高，RTS#拉低，此时Q3和Q4都导通，那么BOOT0为高电平状态，RESET为低电平复位，然后DTR#变低，Q3不导通，复位结束，此时的BOOT0为高电平，经过4个SYSCLK，BOOT0的状态会被锁存，变成串口下载模式，那么就会启用串口下载程序，开始和FlyMCU下载软件进行通信，进行代码下载；

　　> 等到代码下载结束时，RTS#先变高电平，然后DTR#再变高电平，变成程序启动模式。（这个顺序不能变，如果DTR#先变到高电平，Q3会导通，RESET变成低电平，这样又会产生复位。）

　　时序如下图：

 ![img](https://images2018.cnblogs.com/blog/1184137/201802/1184137-20180227141606077-443434351.png)

关于疑问1：启动模式的切换，是在复位后的4个SYSCLK后，再确定的，那么在烧录完成后，为什么没有复位，就切换成了程序启动模式呢?不是应该还是在烧录模式吗？

小白看了下网上的解答，说是烧录软件已经帮我们配置好了，结果就是在烧写完成后会有报告：“成功从08000000开始运行”。这个回答小白不认同，总之结果是这样，要搞清楚，就只能有时间再用示波器抓下时序看看，这个疑问暂存吧！

关于疑问2：软件配置为“DTR的低电平复位，RTS高电平进BootLoader”，这样配置不是进入串口下载模式了吗？

小白看了下网上的解答，说是IC在实际操作时引脚的变化为“DTR#拉高，RTS#拉低”，即软件设置和实际情况是相反的。

![img](https://images2018.cnblogs.com/blog/1184137/201802/1184137-20180227143609287-812504418.png)