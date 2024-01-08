
#include "Dio.h"

bool CAN_WakeUpSignal = false;

static GPIO_TypeDef* Dio_PORTTypeConvert(GPIO_Type GPIO_Base)
{
    switch(GPIO_Base)
    {
        case PORT_A:
            return GPIOA;
        case PORT_B:
            return GPIOB;
        case PORT_C:
            return GPIOC;
        case PORT_D:
            return GPIOD;
        case PORT_E:
            return GPIOE;
        case PORT_F:
            return GPIOF;
        case PORT_G:
            return GPIOG;
        case PORT_H:
            return GPIOH;
        case PORT_I:
            return GPIOI;
        case PORT_J:
            return GPIOJ;
        case PORT_K:
            return GPIOK;
    }
}

static GPIO_PORTState Dio_PORTStateConvert(GPIO_PinState GPIO_State)
{
    switch(GPIO_State)
    {
        case GPIO_PIN_RESET:
            return PORT_RESET;
        case GPIO_PIN_SET:
            return PORT_SET;
        default:
            return PORT_Err;
    }
}

GPIO_PORTState Dio_ReadChannel(GPIO_Pins GPIO_Cfg)
{
    GPIO_TypeDef* PORT_ACT = Dio_PORTTypeConvert(GPIO_Cfg.GPIO_Base);
    GPIO_PinState PORT_State = HAL_GPIO_ReadPin(PORT_ACT, GPIO_Cfg.GPIO_Pin);
    return Dio_PORTStateConvert(PORT_State);
}

void Dio_WriteChannel(GPIO_Pins GPIO_Cfg, GPIO_PORTState GPIO_State)
{
    GPIO_TypeDef* PORT_ACT = Dio_PORTTypeConvert(GPIO_Cfg.GPIO_Base);
    HAL_GPIO_WritePin(PORT_ACT, GPIO_Cfg.GPIO_Pin, Dio_PORTStateConvert(GPIO_State));
}


bool CAN_GetWakeUpSignal(void)
{
    return CAN_WakeUpSignal;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch(GPIO_Pin)
    {
        case KEY_0_Pin:
            CAN_WakeUpSignal = !CAN_WakeUpSignal;
            if(CAN_WakeUpSignal)
            {
                CanNM_SetWakeUpReason(NM_Wakeup_Local);
            }else{
                CanNM_ClearWakeUpReason(NM_Wakeup_Local);
            }
            break;
    }
  
}
