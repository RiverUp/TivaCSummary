#include "Motor.h"

// pc4 pb6输出 pa2使能 pd2(左) pd3(右）方向,低电平正转，高电平反转
void initMotor(void)
{
    // pwm
    SysCtlPWMClockSet(SYSCTL_PWMDIV_32);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PC4_M0PWM6);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    // 设置PWM0模块的第三个发生器为向下计数与不同步计数
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
                    PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // 方向
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_DIR_MODE_OUT);
    GPIOPadConfigSet(GPIO_PORTD_BASE,
                     GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD); // 推挽输出

    // 使能
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_DIR_MODE_OUT);
    GPIOPadConfigSet(GPIO_PORTA_BASE,
                     GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD); // 推挽输出
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);                       // PA2输出低电平
}

/*
 *关闭电机
 */
void turnOffMotor(void)
{
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0); // PA2输出低电平
}

/*
 * 打开电机
 */
void turnOnMotor(void)
{
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2); // PA2输出高电平
}

void setPWM(int left, int right)
{
    // 正负决定方向
    if (left > 0)
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
    else
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
    if (right > 0)
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
    else
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3);

    if (left == 0)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, 0);
    }
    else
    {
        int pwm_left = 1250000 / abs(left);
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, pwm_left);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_0) * 0.1 - 1);
    }
    if (right == 0)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, 0);
    }
    else
    {
        int pwm_right = 1250000 / abs(right);
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, pwm_right);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, PWMGenPeriodGet(PWM0_BASE, PWM_GEN_3) * 0.1 - 1);
    }

    // 使能第六个通道
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, true);
    // 使能第零个通道
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);
    // 使能第三个发生器
    PWMGenEnable(PWM0_BASE, PWM_GEN_3);
    // 使能第零个发生器
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}
