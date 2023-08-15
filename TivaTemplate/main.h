#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_uart.h"
#include "inc/hw_pwm.h"
#include "inc/hw_adc.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ssi.h"
#include "inc/hw_qei.h"
#include "inc/tm4c123gh6pm.h"

#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "driverlib/i2c.h"
#include "driverlib/eeprom.h"

#include "Serial.h"
#include "Flag.h"
#include "BlueTooth.h"
#include "Lights.h"
#include "OpenmvTrack.h"
#include "Control.h"
#include "Key.h"
#include "Motor.h"
#include "JY62.h"
#include "Hcsr04.h"
#include "DelayStruct.h"
#include "Oled.h"
#include "Encoder.h"
#include "Motor2.h"
#include "BatteryVoltage.h"
#include "Sg90.h"
#include "Beep.h"
#include "EEPRom.h"

// 中断优先级定义
#define USER_INT0 0x00
#define USER_INT1 0x20
#define USER_INT2 0x40
#define USER_INT3 0x60
#define USER_INT4 0x80
#define USER_INT5 0xA0
// 延时函数定义
#define delayms(xms) SysCtlDelay(xms * (80000 / 3)) /*ms级延时*/
#define delayus(xus) SysCtlDelay(xus * (80 / 3))    /*ms级延时*/

#define RIGHT 0
#define LEFT 1

#endif
