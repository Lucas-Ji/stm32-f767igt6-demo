#ifndef DIO_H_INCLUDED
#define DIO_H_INCLUDED

#include <stdbool.h>
#include "stm32f7xx_hal_gpio.h"
#include "gpio.h"
#include "CanNm.h"

typedef enum
{
  PORT_RESET = 0,
  PORT_SET,
  PORT_Err
} GPIO_PORTState;

typedef enum
{
    PORT_A,
    PORT_B,
    PORT_C,
    PORT_D,
    PORT_E,
    PORT_F,
    PORT_G,
    PORT_H,
    PORT_I,
    PORT_J,
    PORT_K,
} GPIO_Type;

typedef struct
{
    GPIO_Type GPIO_Base;
    uint8_t GPIO_Pin;
} GPIO_Pins;

extern bool CAN_GetWakeUpSignal(void);
extern GPIO_PORTState Dio_ReadChannel(GPIO_Pins GPIO_Cfg);
extern void Dio_WriteChannel(GPIO_Pins GPIO_Cfg, GPIO_PORTState GPIO_State);

#endif