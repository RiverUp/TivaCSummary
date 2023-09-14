# Uart串口

tivac123gx开发板的uart引脚有很多，这是他的优点之一

openmv与主板的通信就是通过uart协议完成的，所以我们这里以与openmv通信的代码为例，介绍tivac123gx具备的功能

## 初始化

我们选用了主板中的Uart2与openmv通信，对应引脚为PD6（rx）、PD7（tx）

*PD7是锁住的，使用之前不要忘记解锁*

### 代码

```c
void initOpenmvTrack()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    /* Unlock PD7 */
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0x00;

    GPIOPinConfigure(GPIO_PD6_U2RX);
    GPIOPinConfigure(GPIO_PD7_U2TX);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    UARTFIFODisable(UART2_BASE);
    IntEnable(INT_UART2);
    UARTIntEnable(UART2_BASE, UART_INT_RX);
}
```

`UARTConfigSetExpClk(UART2_BASE,SysCtlClockGet(),115200, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));`这句话将uart2的波特率设置为了115200

## 中断

消息的接收自然要用到中断

中断可以参考前面的按键部分，要么注册，要么修改 `tm4c123gh6pm_startup_ccs.c`文件

处理中断的格式一般都是判断是什么事件触发的中断，然后清除中断位，接着处理中断

### 代码

```c
void UART2IntHandler(void)
{
    uint32_t u32IntStatus = UARTIntStatus(UART2_BASE, true);
    UARTIntClear(UART2_BASE, u32IntStatus);

    if (u32IntStatus & UART_INT_RX)
    {
	//xxx处理代码
    }
}
```

## 发送

这里发送比较简单，就是发送一个字符串

可以先用 `sprintf()` 处理好，然后作为参数

### 代码

```c
void sendMsgToOpenmvTrack(char *msg)
{
    while (*msg)
    {
        UARTCharPut(UART2_BASE, *msg++);
    }
}
```

# 接收Openmv

一般来说，openmv发给我们的信息都是具有帧头帧尾的，所以我们需要处理一下才能得到有效的数据。

正常的数据格式是0x2c,0x12,有效数据,······,有效数据,0x5B

### 代码

在中断中完整的接收代码如下：

```c
uint8_t Cx = 0, Cy = 0, Ci = 0;
void UART2IntHandler(void)
{
    uint32_t u32IntStatus = UARTIntStatus(UART2_BASE, true);
    UARTIntClear(UART2_BASE, u32IntStatus);

    uint8_t com_data;
    uint8_t i;
    static uint8_t RxCounter1 = 0;
    static uint16_t RxBuffer1[7] = {0};
    static uint8_t RxState = 0;
    static uint8_t RxFlag1 = 0;

    if (u32IntStatus & UART_INT_RX)
    {
        com_data = UARTCharGet(UART2_BASE);
        if (RxState == 0 && com_data == 0x2C) // 0x2c帧头
        {
            // sendMsgBySerial("openmv");
            RxState = 1;
            RxBuffer1[RxCounter1++] = com_data;
        }

        else if (RxState == 1 && com_data == 0x12) // 0x12帧头
        {
            RxState = 2;
            RxBuffer1[RxCounter1++] = com_data;
        }

        else if (RxState == 2)
        {
            RxBuffer1[RxCounter1++] = com_data;

            if (RxCounter1 >= 7 || com_data == 0x5B) // RxBuffer1接受满了,接收数据结束
            {
                RxState = 3;
                RxFlag1 = 1;

                Cx = RxBuffer1[RxCounter1 - 4];
                Cy = RxBuffer1[RxCounter1 - 3];
                Ci = RxBuffer1[RxCounter1 - 2];
            }
        }
        else if (RxState == 3) // 检测是否接受到结束标志
        {
            if (RxBuffer1[RxCounter1 - 1] == 0x5B)
            {
                // USART_ITConfig(USART2, USART_IT_RXNE, DISABLE); // 关闭DTSABLE中断
                if (RxFlag1)
                {
		    //处理代码，业务逻辑
                    OpenmvTrackReadOnceFlag = true;
                }
                RxFlag1 = 0;
                RxCounter1 = 0;
                RxState = 0;
                // USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
            }
            else // 接收错误
            {
                RxState = 0;
                RxCounter1 = 0;
                for (i = 0; i < 6; i++)
                {
                    RxBuffer1[i] = 0x00; // 将存放数据数组清零
                }
            }
        }

        else // 接收异常
        {
            RxState = 0;
            RxCounter1 = 0;
            for (i = 0; i < 6; i++)
            {
                RxBuffer1[i] = 0x00; // 将存放数据数组清零
            }
        }
    }
}
```

处理完业务逻辑之后，将 `OpenmvTrackReadOnceFlag`置 `true` 然后可以在 `main()`主循环中处理

```c
while(1)
{
	if(OpenmvTrackReadOnceFlag)
	{
	   //业务逻辑
	   OpenmvTrackReadOnceFlag=false;
	}
}
```

# 其他Uart

还有很多设备也需要uart通信，比如最基础、常用的usb的串口调试（Uart1），和这里openmv选用的串口的初始化、中断回调、发送信息是完全一致的。*在文件中的Serial.c中进行了相关定义*
